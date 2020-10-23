/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     lwm2m_client
 *
 * @file
 * @brief       LwM2M client implementation using Wakaama
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <string.h>

#include "timex.h"

#include "liblwm2m.h"

#include "lwm2m_platform.h"
#include "lwm2m_client.h"
#include "lwm2m_client_config.h"
#include "lwm2m_client_connection.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Determines if there has been a reboot request on the device object
 *
 * @note This function is implemented in object_device.c
 *
 * @return true Reboot has been requested
 * @return false Reboot has not been requested
 */
bool lwm2m_device_reboot_requested(void);

/**
 * @brief Thread with the main loop for receiving packets and stepping the LwM2M
 *        FSM.
 *
 * @param arg ignored
 */
static void *_lwm2m_client_run(void *arg);


static char _lwm2m_client_stack[THREAD_STACKSIZE_MAIN +
                                THREAD_EXTRA_STACKSIZE_PRINTF];
static lwm2m_client_data_t *_client_data;

void lwm2m_client_init(lwm2m_client_data_t *client_data)
{
    (void)client_data;
    lwm2m_platform_init();
}

lwm2m_context_t *lwm2m_client_run(lwm2m_client_data_t *client_data,
                                  lwm2m_object_t *obj_list[],
                                  uint16_t obj_numof)
{
    int res;

    _client_data = client_data;
    _client_data->local_ep.family = AF_INET6;
    _client_data->local_ep.netif = SOCK_ADDR_ANY_NETIF;

    /* create sock for UDP server */
    _client_data->local_ep.port = atoi(CONFIG_LWM2M_LOCAL_PORT);
    if (sock_udp_create(&_client_data->sock, &_client_data->local_ep, NULL, 0)) {
        DEBUG("[lwm2m_client_run] Can't create server socket\n");
        return NULL;
    }

    /* initiate LwM2M */
    _client_data->lwm2m_ctx = lwm2m_init(_client_data);
    if (!_client_data->lwm2m_ctx) {
        DEBUG("[lwm2m_client_run] Failed to initiate LwM2M\n");
        return NULL;
    }

    res = lwm2m_configure(_client_data->lwm2m_ctx, CONFIG_LWM2M_DEVICE_NAME, NULL,
                          CONFIG_LWM2M_ALT_PATH, obj_numof, obj_list);
    if (res) {
        DEBUG("[lwm2m_client_run] Failed to configure LwM2M\n");
        return NULL;
    }

    _client_data->pid = thread_create(_lwm2m_client_stack,
                                     sizeof(_lwm2m_client_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     _lwm2m_client_run,
                                     NULL,
                                     "LwM2M client");
    return _client_data->lwm2m_ctx;
}

static void *_lwm2m_client_run(void *arg)
{
    (void) arg;
    time_t reboot_time = 0;
    while (1) {
        time_t tv = LWM2M_CLIENT_MIN_REFRESH_TIME;
        uint8_t rcv_buf[LWM2M_CLIENT_RCV_BUFFER_SIZE];
        ssize_t rcv_len = sizeof(rcv_buf);
        sock_udp_ep_t remote;

        if (lwm2m_device_reboot_requested()) {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time) {
                DEBUG("reboot requested; rebooting in %u seconds\n",
                        LWM2M_CLIENT_REBOOT_TIME);
                reboot_time = tv_sec + LWM2M_CLIENT_REBOOT_TIME;
            }
            if (reboot_time < tv_sec) {
                DEBUG("reboot time expired, rebooting ...\n");
                pm_reboot();
            }
            else {
                tv = reboot_time - tv_sec;
            }
        }

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some
         * packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the
         * state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        lwm2m_step(_client_data->lwm2m_ctx, &tv);
        DEBUG(" -> State: ");
        switch (_client_data->lwm2m_ctx->state) {
            case STATE_INITIAL:
                DEBUG("STATE_INITIAL\n");
                break;
            case STATE_BOOTSTRAP_REQUIRED:
                DEBUG("STATE_BOOTSTRAP_REQUIRED\n");
                break;
            case STATE_BOOTSTRAPPING:
                DEBUG("STATE_BOOTSTRAPPING\n");
                break;
            case STATE_REGISTER_REQUIRED:
                DEBUG("STATE_REGISTER_REQUIRED\n");
                break;
            case STATE_REGISTERING:
                DEBUG("STATE_REGISTERING\n");
                break;
            case STATE_READY:
                DEBUG("STATE_READY\n");
                if (tv > LWM2M_CLIENT_MIN_REFRESH_TIME) {
                    tv = LWM2M_CLIENT_MIN_REFRESH_TIME;
                }
                break;
            default:
                DEBUG("Unknown...\n");
                break;
        }

        DEBUG("Waiting for UDP packet on port: %d\n", _client_data->sock.local.port);
        rcv_len = sock_udp_recv(&_client_data->sock, &rcv_buf, sizeof(rcv_buf),
                                tv * US_PER_SEC, &remote);
        DEBUG("sock_udp_recv()\n");
        if (rcv_len > 0) {
            DEBUG("Finding connection\n");
            lwm2m_client_connection_t *conn = lwm2m_client_connection_find(
                                            _client_data->conn_list, &remote);
            if (conn) {
                DEBUG("lwm2m_connection_handle_packet(%i)\n", (int)rcv_len);
                int result = lwm2m_connection_handle_packet(conn, rcv_buf,
                                                            rcv_len,
                                                            _client_data);
                if (0 != result) {
                    DEBUG("error handling message %i\n", result);
                }
            }
            else {
                DEBUG("Could not find incoming connection\n");
            }
        }
        else if ((rcv_len < 0) &&
                 ((rcv_len != -EAGAIN) && (rcv_len != -ETIMEDOUT))) {
            DEBUG("Unexpected sock_udp_recv error code %i\n", (int)rcv_len);
        }
        else {
            DEBUG("UDP error code: %i\n", (int)rcv_len);
        }
    }
    return NULL;
}
