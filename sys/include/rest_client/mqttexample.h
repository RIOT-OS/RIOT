/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_mqttexample
 * @{
 *
 * @file
 * @brief       Helper functions wolfMQTT interactions
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
*/

#ifndef REST_CLIENT_MQTTEXAMPLE_H
#define REST_CLIENT_MQTTEXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rest_client.h"

#ifndef CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS
    #define CONFIG_REST_CLIENT_MQTT_CMD_TIMEOUT_MS          30000
#endif

#ifndef CONFIG_REST_CLIENT_MQTT_CON_TIMEOUT_MS
    #define CONFIG_REST_CLIENT_MQTT_CON_TIMEOUT_MS          5000
#endif

#ifndef CONFIG_REST_CLIENT_MQTT_DEFAULT_KEEP_ALIVE_SEC
    #define CONFIG_REST_CLIENT_MQTT_DEFAULT_KEEP_ALIVE_SEC  60
#endif

#ifndef CONFIG_REST_CLIENT_MQTT_PRINT_BUFFER_SIZE
    #define CONFIG_REST_CLIENT_MQTT_PRINT_BUFFER_SIZE       180
#endif

#ifndef CONFIG_REST_CLIENT_MQTT_CLIENT_ID
    #define CONFIG_REST_CLIENT_MQTT_CLIENT_ID               "riot-os"
#endif

void mqtt_init_ctx(rest_client_mqtt_context_t *mqtt_context);
void mqtt_free_ctx(rest_client_mqtt_context_t *mqtt_context);

uint16_t mqtt_get_packetid(void);

#ifdef WOLFMQTT_NONBLOCK
int mqtt_check_timeout(int rc, uint32_t *start_sec, uint32_t timeout_sec);
#endif

#ifdef __cplusplus
}
#endif

#endif /* REST_CLIENT_MQTTEXAMPLE_H */
