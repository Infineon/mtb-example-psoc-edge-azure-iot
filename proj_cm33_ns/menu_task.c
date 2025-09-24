/*******************************************************************************
 * File Name: menu_task.c
 *
 * Description: This file contains tasks and functions related to Azure feature
 * task creation and Wi-Fi initialization.
 *
 *******************************************************************************
 * Copyright 2024-2025, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 ******************************************************************************/

/* Header file includes */
#include "azure_common.h"
#include "cybsp.h"
#include "retarget_io_init.h"
#include "cy_log.h"
#include "cyabs_rtos.h"

/* FreeRTOS header file */
#include <FreeRTOS.h>
#include <task.h>
#include "mqtt_iot_common.h"
/* Wi-Fi connection manager header files. */
#include "cy_wcm_error.h"

/* IP address related header files (part of the lwIP TCP/IP stack). */
#include "ip_addr.h"

/* IoT SDK, Secure Sockets, and MQTT initialization */
#include "cy_tcpip_port_secure_sockets.h"
#include "mqtt_main.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
/* MAX connection retries to join WI-FI AP */
#define MAX_CONNECTION_RETRIES                       (10U)

/* Wait between connection retries */
#define WIFI_CONN_RETRY_DELAY_MS                     (50U)
#define ASCII_INTEGER_DIFFERENCE                     (48U)
#define APP_SDIO_INTERRUPT_PRIORITY                  (7U)
#define APP_HOST_WAKE_INTERRUPT_PRIORITY             (2U)
#define APP_SDIO_FREQUENCY_HZ                        (25000000U)
#define SDHC_SDIO_64BYTES_BLOCK                      (64U)
#define RESET_VAL                                    (0U)

/*******************************************************************************
* Global Variables
*******************************************************************************/

static mtb_hal_sdio_t sdio_instance;
static cy_stc_sd_host_context_t sdhc_host_context;
static cy_wcm_config_t wcm_config;

#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)

/* SysPm callback parameter structure for SDHC */
static cy_stc_syspm_callback_params_t sdcardDSParams =
{
    .context   = &sdhc_host_context,
    .base      = CYBSP_WIFI_SDIO_HW
};

/* SysPm callback structure for SDHC*/
static cy_stc_syspm_callback_t sdhcDeepSleepCallbackHandler =
{
    .callback           = Cy_SD_Host_DeepSleepCallback,
    .skipMode           = SYSPM_SKIP_MODE,
    .type               = CY_SYSPM_DEEPSLEEP,
    .callbackParams     = &sdcardDSParams,
    .prevItm            = NULL,
    .nextItm            = NULL,
    .order              = SYSPM_CALLBACK_ORDER
};
#endif

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
cy_rslt_t connect_to_wifi_ap(void);
/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: sdio_interrupt_handler
********************************************************************************
* Summary:
* Interrupt handler function for SDIO instance.
*******************************************************************************/
static void sdio_interrupt_handler(void)
{
    mtb_hal_sdio_process_interrupt(&sdio_instance);
}

/*******************************************************************************
* Function Name: host_wake_interrupt_handler
********************************************************************************
* Summary:
* Interrupt handler function for the host wake up input pin.
*******************************************************************************/
static void host_wake_interrupt_handler(void)
{
    mtb_hal_gpio_process_interrupt(&wcm_config.wifi_host_wake_pin);
}

/*******************************************************************************
* Function Name: app_sdio_init
********************************************************************************
* Summary:
* This function configures and initializes the SDIO instance used in 
* communication between the host MCU and the wireless device.
*******************************************************************************/
static void app_sdio_init(void)
{
    cy_rslt_t result;
    mtb_hal_sdio_cfg_t sdio_hal_cfg;
    
    cy_stc_sysint_t sdio_intr_cfg =
    {
        .intrSrc = CYBSP_WIFI_SDIO_IRQ,
        .intrPriority = APP_SDIO_INTERRUPT_PRIORITY
    };

    cy_stc_sysint_t host_wake_intr_cfg =
    {
            .intrSrc = CYBSP_WIFI_HOST_WAKE_IRQ,
            .intrPriority = APP_HOST_WAKE_INTERRUPT_PRIORITY
    };

    /* Initialize the SDIO interrupt and specify the interrupt handler. */
    cy_en_sysint_status_t interrupt_init_status = Cy_SysInt_Init(&sdio_intr_cfg, sdio_interrupt_handler);

    /* SDIO interrupt initialization failed. Stop program execution. */
    if(CY_SYSINT_SUCCESS != interrupt_init_status)
    {
        handle_app_error();
    }

    /* Enable NVIC interrupt. */
    NVIC_EnableIRQ(CYBSP_WIFI_SDIO_IRQ);

    /* Setup SDIO using the HAL object and desired configuration */
    result = mtb_hal_sdio_setup(&sdio_instance, &CYBSP_WIFI_SDIO_sdio_hal_config, NULL, &sdhc_host_context);

    /* SDIO setup failed. Stop program execution. */
    if(CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

    /* Initialize and Enable SD HOST */
    Cy_SD_Host_Enable(CYBSP_WIFI_SDIO_HW);
    Cy_SD_Host_Init(CYBSP_WIFI_SDIO_HW, CYBSP_WIFI_SDIO_sdio_hal_config.host_config, &sdhc_host_context);
    Cy_SD_Host_SetHostBusWidth(CYBSP_WIFI_SDIO_HW, CY_SD_HOST_BUS_WIDTH_4_BIT);

    sdio_hal_cfg.frequencyhal_hz = APP_SDIO_FREQUENCY_HZ;
    sdio_hal_cfg.block_size = SDHC_SDIO_64BYTES_BLOCK;

    /* Configure SDIO */
    mtb_hal_sdio_configure(&sdio_instance, &sdio_hal_cfg);

#if (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP)
    /* SDHC SysPm callback registration */
    Cy_SysPm_RegisterCallback(&sdhcDeepSleepCallbackHandler);
#endif /* (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP) */

    /* Setup GPIO using the HAL object for WIFI WL REG ON  */
    mtb_hal_gpio_setup(&wcm_config.wifi_wl_pin, CYBSP_WIFI_WL_REG_ON_PORT_NUM, CYBSP_WIFI_WL_REG_ON_PIN);

    /* Setup GPIO using the HAL object for WIFI HOST WAKE PIN  */
    mtb_hal_gpio_setup(&wcm_config.wifi_host_wake_pin, CYBSP_WIFI_HOST_WAKE_PORT_NUM, CYBSP_WIFI_HOST_WAKE_PIN);

    /* Initialize the Host wakeup interrupt and specify the interrupt handler. */
    cy_en_sysint_status_t interrupt_init_status_host_wake =  Cy_SysInt_Init(&host_wake_intr_cfg, host_wake_interrupt_handler);

    /* Host wake up interrupt initialization failed. Stop program execution. */
    if(CY_SYSINT_SUCCESS != interrupt_init_status_host_wake)
    {
        handle_app_error();
    }

    /* Enable NVIC interrupt. */
    NVIC_EnableIRQ(CYBSP_WIFI_HOST_WAKE_IRQ);
}

/*******************************************************************************
 * Function Name: connect_to_wifi_ap
 *******************************************************************************
 * Summary:
 *  Connects to Wi-Fi AP using the user-configured credentials, retries up to a
 *  configured number of times until the connection succeeds.
 * 
 * Parameters:
 *  NULL
 *
 * Return:
 *  cy_rslt_t: Returns CY_RSLT_SUCCESS if the Wi-Fi AP connection is successful
 *
 ******************************************************************************/
cy_rslt_t connect_to_wifi_ap(void)
{
    cy_wcm_connect_params_t wifi_conn_param;
    cy_wcm_ip_address_t ip_address;
    cy_rslt_t result;

    /* Variable to track the number of connection retries to the Wi-Fi AP
     * specified by WIFI_SSID macro. */
    uint32_t conn_retries = 0;

    app_sdio_init();

    wcm_config.interface = CY_WCM_INTERFACE_TYPE_STA;
    wcm_config.wifi_interface_instance = &sdio_instance;

    /* Initialize Wi-Fi connection manager. */
    result = cy_wcm_init(&wcm_config);
    if(CY_RSLT_SUCCESS != result)
    {
        printf("Failed to initialize Wi-Fi Connection Manager.\n");
        handle_app_error();
    }

    printf("\r\nConnecting to Wi-Fi AP...\r\n\n");

    /* Set the Wi-Fi SSID, password and security type. */
    memset(&wifi_conn_param, RESET_VAL, sizeof(cy_wcm_connect_params_t));
    memcpy(wifi_conn_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
    memcpy(wifi_conn_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
    wifi_conn_param.ap_credentials.security = WIFI_SECURITY;

    /* Connect to the Wi-Fi AP */
    for(conn_retries = 0; conn_retries < MAX_CONNECTION_RETRIES; conn_retries++)
    {
        result = cy_wcm_connect_ap( &wifi_conn_param, &ip_address );

        if(CY_RSLT_SUCCESS == result)
        {
            printf("Successfully connected to Wi-Fi network '%s'.\n",
                                wifi_conn_param.ap_credentials.SSID);

            #if(USE_IPV6_ADDRESS)
            /* Get the IPv6 address.*/
                result = cy_wcm_get_ipv6_addr(CY_WCM_INTERFACE_TYPE_STA,
                                              CY_WCM_IPV6_LINK_LOCAL, &ip_address);
                if(CY_RSLT_SUCCESS == result)
                {
                    printf("IPv6 address (link-local) assigned: %s\n",
                            ip6addr_ntoa((const ip6_addr_t*)&ip_address.ip.v6));
                }
            #else
                printf("IPv4 address assigned: %s\n",
                        ip4addr_ntoa((const ip4_addr_t*)&ip_address.ip.v4));

            #endif /* USE_IPV6_ADDRESS */

            return result;
        }


        printf( "\r\nConnection to Wi-Fi network failed with error code %d."
                "Retrying in %d ms...\n", (int) result, WIFI_CONN_RETRY_DELAY_MS );
        vTaskDelay(pdMS_TO_TICKS(WIFI_CONN_RETRY_DELAY_MS));
    }

    printf( "\r\nExceeded maximum Wi-Fi connection attempts\n" );
    return result;
}

/*******************************************************************************
* Function Name: uart_get_data()
********************************************************************************
* Summary:
*         This function reads a single byte from the UART interface,
*         blocking until data is available.
*
* Parameters:
*          value : uart data
*
* Return:
*          None
*
*******************************************************************************/
static void uart_get_data(uint8_t *value)
{
    uint32_t read_value = Cy_SCB_UART_Get(SCB2);
    while (CY_SCB_UART_RX_NO_DATA == read_value)
    {
        read_value = Cy_SCB_UART_Get(SCB2);
    }
    *value = (uint8_t)read_value;
}

/*******************************************************************************
 * Function Name: menu_task
 *******************************************************************************
* Summary:
*  Task to create Azure feature task based on user input.
*
* Parameters:
*  args
*
* Return:
*  void
*
*******************************************************************************/
void menu_task(void *arg)
{
    /* Status variable */
    uint8_t uart_read_value;
    uint8_t uart_read_integer;
    bool valid_option = false;

    cy_rslt_t result = CY_RSLT_SUCCESS;

    result = connect_to_wifi_ap();
    if(CY_RSLT_SUCCESS != result )
    {
        printf("Unable to connect to Access Point\n");
        printf("Terminating Menu Task\n");
        handle_app_error();
    }

    while(!valid_option)
    {
        printf("\n===============================================================\n");
        printf(MENU_AZURE_IOT_HUB);
        printf("\n===============================================================\n");

        /* Reading option number from console */
        uart_get_data(&uart_read_value);
        /* Converting ASCII character to Integer value */
        uart_read_integer = uart_read_value - ASCII_INTEGER_DIFFERENCE;

        switch(uart_read_integer)
        {

            case DEVICE_DEMO:
            {
                printf("Azure Device demo begins\n");
                xTaskCreate(Azure_Device_Demo_app, "Azure_Device_Demo_app",
                        AZURE_TASK_STACK_DEVICE_DEMO_APP, NULL, AZURE_TASK_PRIORITY_DEVICE_DEMO_APP, NULL);
                valid_option = true;
                break;
            }

            case PLUG_N_PLAY:
            {
                printf("\nPlug and Play feature demo begins\n");
                xTaskCreate(Azure_hub_pnp_app, "Azure_hub_pnp_app",
                        AZURE_TASK_STACK_PNP, NULL, AZURE_TASK_PRIORITY_PNP, NULL);
                valid_option = true;
                break;
            }

            default:
            {
                printf("\x1b[2J\x1b[;H");
                printf("\r\nPlease select from the given valid options\r\n");
            break;
            }
        }
    }

    vTaskDelete(NULL);
}

/* [] END OF FILE */
