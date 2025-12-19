/******************************************************************************
* File Name: mqtt_iot_common.c
*
* Description: This file contains implementation of Utility functions for Azure
* sample applications on Infineon platforms.
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

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cyabs_rtos.h"
#include <az_core.h>
#include "mqtt_iot_common.h"

/***********************************************************
* Macros
************************************************************/
#define SECONDS_IN_A_MINUTE                     (60U)
#define MILLISECONDS_IN_A_SECOND                (1000U)

#define IOT_SAMPLE_PRECONDITION_NOT_NULL(arg)   \
  do                                            \
  {                                             \
    if ((arg) == NULL)                          \
    {                                           \
      IOT_SAMPLE_LOG_ERROR("Pointer is NULL."); \
      exit(1);                                  \
    }                                           \
  } while (0)
#define COMPARE_VAL                             (0U)

/***********************************************************
* Constants
************************************************************/
/* MQTT end point */
static az_span const mqtt_url_prefix = AZ_SPAN_LITERAL_FROM_STR("");
static az_span const mqtt_url_suffix = AZ_SPAN_LITERAL_FROM_STR("");

/************************************************************
* Function Prototype
************************************************************/
void build_error_message( char* out_full_message, size_t full_message_buf_size,
                          char const* const error_message, ... );
bool get_az_span( az_span* out_span, char const* const error_message, ... );
uint16_t iot_sample_create_mqtt_endpoint( iot_sample_type type, iot_sample_environment_variables const* env_vars,
                                          char* out_endpoint, size_t endpoint_size );
void iot_sample_sleep_for_seconds( uint32_t seconds );
uint32_t iot_sample_get_epoch_expiration_time_from_minutes( uint32_t minutes );

/************************************************************
* Utility functions
************************************************************/
/******************************************************************************
* Function Name: build_error_message()
*******************************************************************************
* Summary:
*         This function builds full error message.
*
* Parameters:
*          out_full_message : Output buffer for message.
*          full_message_buf_size : Output buffer size.
*          const error_message : Base error message.
*          ... : Optional arguments (unused).
*
* Return:
*          void
*
******************************************************************************/
void build_error_message( char* out_full_message, size_t full_message_buf_size,
                          char const* const error_message, ... )
{
    char const* const append_message = ": az_result return code 0x%08x.";

    size_t message_len = strlen(error_message) + 1;
    strncpy(out_full_message, error_message, full_message_buf_size);
    out_full_message[full_message_buf_size - 1] = 0;
    if (full_message_buf_size > message_len)
    {
        strncat(out_full_message, append_message, full_message_buf_size - message_len);
        out_full_message[full_message_buf_size - 1] = 0;
    }
}

/******************************************************************************
* Function Name: get_az_span()
*******************************************************************************
* Summary:
*         This function retrieves the value of az_span from arguments.
*
* Parameters:
*          out_span : Output az_span pointer.
*          error_message : Format string (unused).
*          ... : Optional arguments (unused).
*
* Return:
*          bool : true if the size of the retrieved az_span is not equal to
*          COMPARE_VAL, false if the size is equal to COMPARE_VAL
*
******************************************************************************/
bool get_az_span( az_span* out_span, char const* const error_message, ... )
{
    va_list args;
    va_start(args, error_message);

    *out_span = va_arg(args, az_span);
    va_end(args);

    if (az_span_size(*out_span) == COMPARE_VAL)
    {
        return false;
    }

    return true;
}

/******************************************************************************
* Function Name: iot_sample_create_mqtt_endpoint()
*******************************************************************************
* Summary:
*         Builds an MQTT endpoint C string for an Azure IoT Hub or provisioning
*         service
*
* Parameters:
*          type: Enumerated type of the sample.
*          env_vars: Pointer to environment variable struct.
*          out_endpoint: Buffer with sufficient capacity to hold the built
*          endpoint. If successful, contains a null-terminated string 
*          of the endpoint.
*          endpoint_size: endpoint_size Size of \p out_endpoint in bytes
*
* Return:
*          uint16_t : Returns the Endpoint URL size on success
*
******************************************************************************/
uint16_t iot_sample_create_mqtt_endpoint( iot_sample_type type, iot_sample_environment_variables const* env_vars,
                                          char* out_endpoint, size_t endpoint_size )
{
    uint16_t required_size = 0;
    IOT_SAMPLE_PRECONDITION_NOT_NULL( env_vars );
    IOT_SAMPLE_PRECONDITION_NOT_NULL( out_endpoint );

    if( type == CY_MQTT_IOT_HUB )
    {
        required_size = az_span_size(mqtt_url_prefix) +
                        az_span_size(env_vars->hub_hostname) +
                        az_span_size(mqtt_url_suffix) +
                        (int32_t)sizeof('\0');
        if ((size_t)required_size > endpoint_size)
        {
            IOT_SAMPLE_LOG_ERROR("Failed to create MQTT endpoint: Buffer is too small.");
            return 0;
        }

        az_span hub_mqtt_endpoint = az_span_create((uint8_t*)out_endpoint, (int32_t)endpoint_size);
        az_span remainder = az_span_copy(hub_mqtt_endpoint, mqtt_url_prefix);
        remainder = az_span_copy(remainder, env_vars->hub_hostname);
        remainder = az_span_copy(remainder, mqtt_url_suffix);
        az_span_copy_u8(remainder, '\0');
    }
    else
    {
        IOT_SAMPLE_LOG_ERROR( "Failed to create MQTT endpoint: Sample type undefined." );
        return 0;
    }

    IOT_SAMPLE_LOG_SUCCESS( "MQTT endpoint created at \"%s\".", out_endpoint );
    return required_size;
}

/******************************************************************************
* Function Name: iot_sample_sleep_for_seconds()
*******************************************************************************
* Summary:
*         This function enters sleep for the given duration in seconds.
*
* Parameters:
*          seconds: Sleep duration in seconds.
*
* Return:
*          void
*
******************************************************************************/
void iot_sample_sleep_for_seconds( uint32_t seconds )
{
    cy_rtos_delay_milliseconds( seconds * MILLISECONDS_IN_A_SECOND );
}

/******************************************************************************
* Function Name: iot_sample_get_epoch_expiration_time_from_minutes()
*******************************************************************************
* Summary:
*         This function calculates epoch expiration time in seconds.
*
* Parameters:
*         minutes : Number of minutes to include in total seconds returned
*
* Return: 
*          uint32_t : Total time in seconds
*
******************************************************************************/
uint32_t iot_sample_get_epoch_expiration_time_from_minutes( uint32_t minutes )
{
    return (uint32_t)( time(NULL) + minutes * SECONDS_IN_A_MINUTE );
}

/* [] END OF FILE */
