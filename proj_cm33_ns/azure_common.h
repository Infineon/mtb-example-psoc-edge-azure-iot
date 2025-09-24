/******************************************************************************
 * File Name: azure_common.h
 *
 * Description: Contains all the common configurations required for the Azure App.
 *
 ********************************************************************************
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
 *******************************************************************************/

#ifndef AZURE_COMMON_H_
#define AZURE_COMMON_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "mqtt_main.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/
/* Macro value 0 indicates that SAS token-based authentication is not used.
 * Macro value 1 indicates that SAS token-based authentication is used.
 */
#define SAS_TOKEN_AUTH                  0U

/*
 * Macro value true indicates that the SAS token needs to be read from
 * the flash memory (from application buffer).
 *
 * Macro value false indicates that the SAS token needs to be read from
 * the secured memory (only for secured platform device).
 */
#define SAS_TOKEN_LOCATION_FLASH        false

#define test_result_t cy_rslt_t
#define TEST_PASS  CY_RSLT_SUCCESS
#define TEST_FAIL  ( -1 )

#define TEST_DEBUG( x )                 printf x
#define TEST_INFO( x )                  printf x
#define TEST_ERROR( x )                 printf x

#define DEBUG_TASK_CREATION_ERROR       (1U)

/* Azure Menu */
#define MENU_AZURE_IOT_HUB                                                              \
        "\n"                                                                            \
        "Please select the index of Azure IoT Hub feature to be tested from below:\n"   \
        "\n"                                                                            \
        "1. Azure Device App (C2D, Telemetry, Methods, Twin)\n"                         \
        "2. PnP (Plug and Play)\n"                                                      \

/* Azure Welcome Message */
#define AZURE_WELCOME_MESSAGE                                                           \
        "\r\n==================================================================\n"      \
        "\r\n Welcome to the Azure IoT Hub Demo \n"                                     \
        "\r\n==================================================================\n"      \

/* Stack sizes for Menu task */
#define MENU_TASK_STACK                         (1024U * 5U)

/* Priorities for Menu task */
#define MENU_TASK_PRIORITY                      (5U)

/* Stack sizes for Azure features tasks */
#define AZURE_TASK_STACK_PNP                    (1024U * 5U)
#define AZURE_TASK_STACK_DEVICE_DEMO_APP        (1024U * 5U)
#define AZURE_TASK_STACK_METHODS                (1024U * 5U)
#define AZURE_TASK_STACK_TWIN                   (1024U * 5U)

/* Priorities for Azure features tasks */
#define AZURE_TASK_PRIORITY_PNP                 (5U)
#define AZURE_TASK_PRIORITY_DEVICE_DEMO_APP     (5U)
#define AZURE_TASK_PRIORITY_METHODS             (5U)
#define AZURE_TASK_PRIORITY_TWIN                (5U)

/******************************************************************************
 * Global Variables
 *******************************************************************************/
#ifdef CY_TFM_PSA_SUPPORTED
extern struct                                   ns_mailbox_queue_t ns_mailbox_queue;
#endif

typedef enum
{
    DEVICE_DEMO=1,
    PLUG_N_PLAY
} azure_features_t;

/******************************************************************************
 * Function Prototypes
 *******************************************************************************/
void Azure_hub_pnp_app(void *arg);
void menu_task(void *arg);
void Azure_Device_Demo_app(void *arg);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* AZURE_COMMON_H_ */

/* [] END OF FILE */
