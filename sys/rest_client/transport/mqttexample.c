/*
 * Copyright (C) 2006-2022 wolfSSL Inc.
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_transport_mqtt
 *
 * @{
 * @file
 * @brief       Utility for wolfMQTT
 * @note        Code was derived from the wolfMQTT repository (see https://github.com/wolfSSL/wolfMQTT/blob/master/examples/mqttexample.c)
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

/* Include the autoconf generated config.h */
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "wolfmqtt/mqtt_client.h"
#include "rest_client/mqttexample.h"
#include "rest_client/mqttnet.h"

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"


/* locals */
static volatile uint16_t _packet_id_last;

void mqtt_free_ctx(rest_client_mqtt_context_t *mqtt_context)
{
    if (mqtt_context == NULL) {
        return;
    }

#ifdef WOLFMQTT_CLIENT_ID
    if (mqtt_context->dynamic_client_id && mqtt_context->client_id) {
        WOLFMQTT_FREE((char *)mqtt_context->client_id);
        mqtt_context->client_id = NULL;
    }
#endif
}

uint16_t mqtt_get_packetid(void)
{
    /* Check rollover */
    if (_packet_id_last >= MAX_PACKET_ID) {
        _packet_id_last = 0;
    }

    return ++_packet_id_last;
}

#ifdef WOLFMQTT_NONBLOCK
#include <time.h>

static uint32_t mqtt_get_timer_seconds(void)
{
    uint32_t timer_sec = 0;

    /* Posix style time */
    timer_sec = (uint32_t)time(0);

    return timer_sec;
}

int mqtt_check_timeout(int rc, uint32_t *start_sec, uint32_t timeout_sec)
{
    uint32_t elapsed_sec;

    /* if start seconds not set or is not continue */
    if (*start_sec == 0 || rc != MQTT_CODE_CONTINUE) {
        *start_sec = mqtt_get_timer_seconds();
        return rc;
    }

    elapsed_sec = mqtt_get_timer_seconds();
    if (*start_sec < elapsed_sec) {
        elapsed_sec -= *start_sec;
        if (elapsed_sec >= timeout_sec) {
            *start_sec = mqtt_get_timer_seconds();
            DEBUG("Timeout timer %d seconds\n", timeout_sec);
            return MQTT_CODE_ERROR_TIMEOUT;
        }
    }

    return rc;
}
#endif /* WOLFMQTT_NONBLOCK */
