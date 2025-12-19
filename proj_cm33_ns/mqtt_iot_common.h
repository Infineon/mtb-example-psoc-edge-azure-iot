/******************************************************************************
* File Name: mqtt_iot_common.h
*
* Description: This file contains header file for Azure sample applications
* utility functions on Infineon platforms.
*
********************************************************************************
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
*******************************************************************************/

#ifndef IOT_SAMPLE_COMMON_H
#define IOT_SAMPLE_COMMON_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <az_core.h>
#include "cy_wcm.h"

/*******************************************************************************
* Macros
********************************************************************************/
#define IOT_SAMPLE_APP_BUFFER_SIZE_IN_BYTES       (256U)
#define IOT_SAMPLE_SAS_KEY_DURATION_TIME_DIGITS   (4U)
#define IOT_SAMPLE_MQTT_PUBLISH_QOS               (0U)

/* Logging */
#define IOT_SAMPLE_LOG_ERROR(...)                                                  \
  do                                                                               \
  {                                                                                \
    (void)fprintf(stderr, "\n\rERROR :%s:%s():%d: ", __FILE__, __func__, __LINE__); \
    (void)fprintf(stderr, __VA_ARGS__);                                            \
    (void)fprintf(stderr, "\n\r");                                                   \
    fflush(stdout);                                                                \
    fflush(stderr);                                                                \
  } while (0)

#define IOT_SAMPLE_LOG_SUCCESS(...) \
  do                                \
  {                                 \
    (void)printf("\n\rSUCCESS: ");  \
    (void)printf(__VA_ARGS__);      \
    (void)printf("\n\r");           \
  } while (0)

#define IOT_SAMPLE_LOG(...)    \
  do                           \
  {                            \
    (void)printf("\n\r");      \
    (void)printf(__VA_ARGS__); \
    (void)printf("\n\r");      \
  } while (0)

#define IOT_SAMPLE_LOG_AZ_SPAN(span_description, span)                                           \
  do                                                                                             \
  {                                                                                              \
    (void)printf("\n\r%s ", span_description);                                                   \
    (void)fwrite((char*)az_span_ptr(span), sizeof(uint8_t), (size_t)az_span_size(span), stdout); \
    (void)printf("\n\r");                                                                          \
  } while (0)

void build_error_message(char* out_full_message, size_t full_message_buf_size, char const* const error_message, ...);
bool get_az_span(az_span* out_span, char const* const error_message, ...);

#define IOT_SAMPLE_EXIT_IF_AZ_FAILED(azfn, ...)                                            \
  do                                                                                       \
  {                                                                                        \
    az_result const result = (azfn);                                                       \
                                                                                           \
    if (az_result_failed(result))                                                          \
    {                                                                                      \
      char full_message[256];                                                              \
      build_error_message(full_message, sizeof(full_message), __VA_ARGS__);                \
                                                                                           \
      az_span span;                                                                        \
      bool has_az_span = get_az_span(&span, __VA_ARGS__, AZ_SPAN_EMPTY);                   \
      if (has_az_span)                                                                     \
      {                                                                                    \
        IOT_SAMPLE_LOG_ERROR(full_message, az_span_size(span), az_span_ptr(span), result); \
      }                                                                                    \
      else                                                                                 \
      {                                                                                    \
        IOT_SAMPLE_LOG_ERROR(full_message, result);                                        \
      }                                                                                    \
      exit(1);                                                                             \
    }                                                                                      \
  } while (0)

/***********************************************************
* Global Variables
************************************************************/
typedef struct
{
  az_span hub_device_id;
  az_span hub_hostname;
  az_span hub_sas_key;
  az_span provisioning_id_scope;
  az_span provisioning_registration_id;
  az_span provisioning_sas_key;
  az_span x509_cert_pem_file_path;
  az_span x509_trust_pem_file_path;
  uint32_t sas_key_duration_minutes;
} iot_sample_environment_variables;

typedef struct
{
  uint8_t* device_id;
  uint16_t device_id_len;
  uint8_t* sas_token;
  uint16_t sas_token_len;
} iot_sample_credentials;

typedef enum
{
  CY_MQTT_IOT_HUB,
  CY_MQTT_IOT_PROVISIONING
} iot_sample_type;

typedef enum
{
  CY_MQTT_IOT_HUB_C2D_SAMPLE,
  CY_MQTT_IOT_HUB_METHODS_SAMPLE,
  CY_MQTT_IOT_HUB_PNP_COMPONENT_SAMPLE,
  CY_MQTT_IOT_HUB_PNP_SAMPLE,
  CY_MQTT_IOT_HUB_SAS_TELEMETRY_SAMPLE,
  CY_MQTT_IOT_HUB_TELEMETRY_SAMPLE,
  CY_MQTT_IOT_HUB_TWIN_SAMPLE,
  CY_MQTT_IOT_PROVISIONING_SAMPLE,
  CY_MQTT_IOT_PROVISIONING_SAS_SAMPLE
} iot_sample_name;

extern bool is_device_operational;

uint16_t iot_sample_create_mqtt_endpoint(
    iot_sample_type type,
    iot_sample_environment_variables const* env_vars,
    char* endpoint,
    size_t endpoint_size);

void iot_sample_sleep_for_seconds(uint32_t seconds);

uint32_t iot_sample_get_epoch_expiration_time_from_minutes(uint32_t minutes);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* IOT_SAMPLE_COMMON_H */

/* [] END OF FILE */
