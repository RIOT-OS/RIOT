/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_mqttnet
 * @{
 *
 * @file
 * @brief       Definitions for basic wolfMQTT interactions
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
*/

#ifndef REST_CLIENT_MQTTNET_H
#define REST_CLIENT_MQTTNET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rest_client/mqttexample.h"

#ifndef DEFAULT_MQTT_HOST
#define DEFAULT_MQTT_HOST       "mqtt.eclipse.org" /* broker.hivemq.com */
#endif

int mqttnet_init(MqttNet *net, rest_client_mqtt_context_t *mqtt_context);
int mqttnet_deinit(MqttNet *net);

#ifdef WOLFMQTT_SN
int mqttnet_sn_init(MqttNet *net, rest_client_mqtt_context_t *mqtt_context);
#endif

#ifdef __cplusplus
}
#endif

#endif /* REST_CLIENT_MQTTNET_H */
