/******************************************************************************
 * File Name: mqtt_main.h
 *
 * Description: Contains all the Azure IoT device configurations required by the
 * Azure App.
 *
 *******************************************************************************
 * (c) 2024-2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 ******************************************************************************/

#ifndef CY_MQTT_APP_H_
#define CY_MQTT_APP_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Wi-Fi Credentials: Modify WIFI_SSID and WIFI_PASSWORD to match your Wi-Fi network
 * Credentials.
 */
/* Wi-Fi SSID */
#define WIFI_SSID                               "Replace this string by WiFi SSID"
/* Wi-Fi PASSWORD */
#define WIFI_PASSWORD                           "Replace this string by WiFi PASSWORD"

/* Security type of the Wi-Fi access point. See 'cy_wcm_security_t' structure
 * in "cy_wcm.h" for more details.
 */
/* default option - CY_WCM_SECURITY_WPA2_AES_PSK */
#define WIFI_SECURITY                           (CY_WCM_SECURITY_WPA2_AES_PSK)

#define MAX_WIFI_RETRY_COUNT                    ( 3U )

/* Modify the MQTT client identifiers for Azure broker from azure account */

/* Unique Device identifier, used only with SAS Authentication 
 * Device identity used for device authentication and access control
*/
#define MQTT_CLIENT_IDENTIFIER_AZURE_SAS        "Replace this string by device ID generated from Azure cloud"
/* Unique Device identifier, used only with X509 Authentication,
 * Device identitifier used for device authentication and access control
*/
#define MQTT_CLIENT_IDENTIFIER_AZURE_CERT       "Replace this string by device ID generated from Azure cloud"

/*
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic name starts with the client identifier to ensure that each demo
 * interacts with a unique topic name.
 */
#define MQTT_TOPIC                              "Test_Topic"
#define MQTT_TOPIC_AZURE_C2D                    "devices/+/messages/devicebound/#"
#define MQTT_TOPIC_AZURE_TELEMETRY_SAS          "devices/" MQTT_CLIENT_IDENTIFIER_AZURE_SAS "/messages/events/"
#define MQTT_TOPIC_AZURE_TELEMETRY_CERT         "devices/" MQTT_CLIENT_IDENTIFIER_AZURE_CERT "/messages/events/"

/* Azure Server endpoints used for the demos */
/* "Replace this string by generated IoT Host name from Azure cloud".
 * Example: my-azure-hubname.azure-devices.net */
#define IOT_DEMO_SERVER_AZURE                   "<hub_name>.azure-devices.net"
#define IOT_DEMO_PORT_AZURE_S                   ( 8883 )

/*
 * The following user name is generated using the Azure C SDK API.
 * The following macro will be used in case of direct connection
 * (without Azure SDK).
 */
/* Update the hub and device name in the string. */
/* Example: IOT_DEMO_SERVER_AZURE "/" "my_azure_dev1" "/?api-version=2018-06-30" */
#define IOT_AZURE_USERNAME                      IOT_DEMO_SERVER_AZURE "/" "<dev_ID>" "/?api-version=2018-06-30"
#define IOT_AZURE_USERNAME_LENGTH               ( ( uint16_t ) ( sizeof( IOT_AZURE_USERNAME ) - 1 ) )

/* Update the Generated SAS Token for Device in Azure IoT Hub, Example: "SharedAccessSignature sr= ... 5560" */
#define IOT_AZURE_PASSWORD                      "Replace this string by generated SAS token."
#define IOT_AZURE_PASSWORD_LENGTH                ( ( uint16_t ) ( sizeof( IOT_AZURE_PASSWORD ) - 1 ) )

/* For DPS application */
/* Replace this string by generated registration ID from Azure portal for DPS*/
#define IOT_AZURE_DPS_REGISTRATION_ID           "Replace this string by generated registration ID from Azure portal for DPS"
#define IOT_AZURE_DPS_REGISTRATION_ID_LEN       ( ( uint16_t ) ( sizeof( IOT_AZURE_DPS_REGISTRATION_ID ) - 1 ) )
/* Replace this string by generated ID Scope from Azure portal for DPS Ex:0ne00****** */
#define IOT_AZURE_ID_SCOPE                      "Replace this string by generated ID scope from Azure portal for DPS"
#define IOT_AZURE_ID_SCOPE_LEN                  ( ( uint16_t ) ( sizeof( IOT_AZURE_ID_SCOPE ) - 1 ) )

#ifdef CY_SECURE_SOCKETS_PKCS_SUPPORT

#define IOT_AZURE_ROOT_CA_LENGTH                ( 0U )
#define IOT_AZURE_CLIENT_CERT_LENGTH            ( 0U )
#define IOT_AZURE_CLIENT_KEY_LENGTH             ( 0U )
#else
#define IOT_AZURE_ROOT_CA_LENGTH                ( ( uint16_t ) ( sizeof( azure_root_ca_certificate ) ) )
#define IOT_AZURE_CLIENT_CERT_LENGTH            ( ( uint16_t ) ( sizeof( azure_client_cert ) ) )
#define IOT_AZURE_CLIENT_KEY_LENGTH             ( ( uint16_t ) ( sizeof( azure_client_key ) ) )
#endif

#ifndef PSA_DEVICEID_UID
/* UID for provisioned device ID */
#define PSA_DEVICEID_UID                        ( 2U )
#endif

#ifndef PSA_SAS_TOKEN_UID
/* UID for provisioned SAS token */
#define PSA_SAS_TOKEN_UID                       ( 3U )
#endif

/* Length of client identifier */
#define MQTT_CLIENT_IDENTIFIER_LENGTH               ( ( uint16_t ) ( sizeof( MQTT_CLIENT_IDENTIFIER ) - 1 ) )
#define MQTT_CLIENT_IDENTIFIER_AWS_LENGTH           ( ( uint16_t ) ( sizeof( MQTT_CLIENT_IDENTIFIER_AWS ) - 1 ) )
#define MQTT_CLIENT_IDENTIFIER_ECLIPSE_LENGTH       ( ( uint16_t ) ( sizeof( MQTT_CLIENT_IDENTIFIER_ECLIPSE ) - 1 ) )
#define MQTT_CLIENT_IDENTIFIER_AZURE_SAS_LENGTH     ( ( uint16_t ) ( sizeof( MQTT_CLIENT_IDENTIFIER_AZURE_SAS ) - 1 ) )
#define MQTT_CLIENT_IDENTIFIER_AZURE_CERT_LENGTH    ( ( uint16_t ) ( sizeof( MQTT_CLIENT_IDENTIFIER_AZURE_CERT ) - 1 ) )

/* Length of client MQTT topic */
#define MQTT_TOPIC_LENGTH                           ( ( uint16_t ) ( sizeof( MQTT_TOPIC ) - 1 ) )
#define MQTT_TOPIC_AZURE_C2D_LENGTH                 ( ( uint16_t ) ( sizeof( MQTT_TOPIC_AZURE_C2D ) - 1 ) )
#define MQTT_TOPIC_AZURE_TELEMETRY_SAS_LENGTH       ( ( uint16_t ) ( sizeof( MQTT_TOPIC_AZURE_TELEMETRY_SAS ) - 1 ) )
#define MQTT_TOPIC_AZURE_TELEMETRY_CERT_LENGTH      ( ( uint16_t ) ( sizeof( MQTT_TOPIC_AZURE_TELEMETRY_CERT ) - 1 ) )

/* MQTT message published in this example */
#define MQTT_TEST_MESSAGE                           "Hello World!"

/* Length of the MQTT message published in this example */
#define MQTT_TEST_MESSAGE_LENGTH                    ( ( uint16_t ) ( sizeof( MQTT_TEST_MESSAGE ) - 1 ) )

/* MQTT will message published in this example */
#define MQTT_TEST_WILL_MESSAGE                      "Will message - World!"

/* Length of the MQTT will message published in this example */
#define MQTT_TEST_WILL_MESSAGE_LENGTH               ( ( uint16_t ) ( sizeof( MQTT_TEST_WILL_MESSAGE ) - 1 ) )

/* Number of PUBLISH messages sent per iteration */
#define MQTT_TEST_PUBLISH_COUNT                     ( 5U )

/* Size of the network buffer for MQTT packets */
#define NETWORK_BUFFER_SIZE                         ( 1024U )

/* Delay between MQTT publishes in seconds */
#define DELAY_BETWEEN_PUBLISHES_SECONDS             ( 1U )

/**
 * @brief Maximum time interval in seconds which is allowed to elapse
 *  between two Control packets.
 *
 *  It is the responsibility of the client to ensure that the interval between
 *  Control packets being sent does not exceed this keepalive value. If no other
 *  Control packets are sent, the client MUST send a PINGREQ packet.
 */
#define MQTT_KEEP_ALIVE_INTERVAL_SECONDS            ( 240U )

/***********************************************************
 * Constants
 ************************************************************/
/* Azure Broker connection Info */
/* The azure_root_ca_certificate field requires the the  DigiCert Global G2 Root certificate CyberTrust Root
 * certificate for Azure IoT, azure_rootCA provided with
 * the application is the  DigiCert Global G2 Root certificate CyberTrust Root
 * certificate.
 *
 * Must follow the below format and include the PEM header and footer:
        "-----BEGIN CERTIFICATE-----\r\n"
        ".........base64 data.......\r\n"
        "-----END CERTIFICATE-------\r\n";
 */
static const char azure_root_ca_certificate[] = 
        "Replace this string with  DigiCert Global G2 Root certificate CyberTrust Root CA";

/* Azure - Device certificate */
/* Must follow the below format and include the PEM header and footer:
        "-----BEGIN CERTIFICATE-----\n"
        ".........base64 data.......\n"
        "-----END CERTIFICATE-------";
 */
static const char azure_client_cert[] = "Replace this string with generated client certificate";

/* Azure - private key */
/* Must follow the below format and include the PEM header and footer:
        "-----BEGIN PRIVATE KEY-----\n"
        ".........base64 data.......\n"
        "-----END PRIVATE KEY-----";
 */
static const char azure_client_key[] = "Replace this string with generated client private key";

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
/* MQTT-supported QoS levels */
typedef enum cy_demo_mqtt_qos
{
    CY_MQTT_QOS_0 = 0,                  /* Delivery at most once */
    CY_MQTT_QOS_1 = 1,                  /* Delivery at least once */
    CY_MQTT_QOS_2 = 2                   /* Delivery exactly once */
} cy_demo_mqtt_qos_t;

typedef enum {
    SECURED_MQTT,                       /* For secured TCP connection */
    NON_SECURED_MQTT                    /* For non-secured TCP connection */
} mqtt_security_flag;

typedef enum {
    AWS_MQTT_BROKER,                    /* For AWS MQTT Broker connection */
    AZURE_MQTT_BROKER,                  /* For Azure MQTT Broker connection */
    ECLIPSE_MOSQUITTO_MQTT_BROKER,      /* For Eclipse Mosquitto MQTT Broker connection */
} mqtt_broker;

typedef enum {
    AUTH_MODE_X509_CERT = 1,            /* For cert authentication. */
    AUTH_MODE_SAS_TOKEN = 2,            /* For SAS token authentication. */
} mqtt_auth_mode;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* CY_MQTT_APP_H_ */

/* [] END OF FILE */
