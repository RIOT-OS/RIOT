/*******************************************************************************
*
* Copyright (c) 2013, 2014 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Benjamin Cabé - Please refer to git log
*    Fabien Fleutot - Please refer to git log
*    Simon Bernard - Please refer to git log
*    Julien Vermillard - Please refer to git log
*    Axel Lorente - Please refer to git log
*    Toby Jaffey - Please refer to git log
*    Bosch Software Innovations GmbH - Please refer to git log
*    Pascal Rieux - Please refer to git log
*    Christian Renz - Please refer to git log
*    Ricky Liu - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without
   modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
   OF
   THE POSSIBILITY OF SUCH DAMAGE.

   David Navarro <david.navarro@intel.com>
   Bosch Software Innovations GmbH - Please refer to git log

 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Main loop for Wakaama example client.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include "memarray_limits.h"

#include "connection.h"
#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "lwm2mconfig.h"

#include "periph/pm.h"

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

/* Taken from examples/dtls-echo/dtls-server.c */
#define READER_QUEUE_SIZE (8U)

#ifdef MODULE_TINYDTLS
#define RCVD_SIZE DTLS_MAX_BUF
#else /* MODULE_TINYDTLS */
#define RCVD_SIZE 200
#endif /* MODULE_TINYDTLS */

/* Time in seconds to reboot after from server request */
#define REBOOT_TIME 5

/* Time in seconds to wait for a server response.
 * Also controls the main loop max interval,
 * which might be relevant for sensor polling.
 */
#define RESPONSE_TIMEOUT 20;

static int g_quit = 0;

#define OBJ_COUNT 4
lwm2m_object_t *obj_array[OBJ_COUNT];

/* only backup security and server objects */
#define BACKUP_OBJECT_COUNT 2
lwm2m_object_t *backup_object_array[BACKUP_OBJECT_COUNT];

typedef struct {
    lwm2m_object_t *security_obj;
    lwm2m_object_t *server_object;
    sock_udp_t sock;
    lwm2m_connection_t *conn_list;
    lwm2m_context_t *lwm2mH;
    int address_family;
} client_data_t;

#ifdef BOARD_NATIVE
void handle_sigint(int signum)
{
    (void)signum;
    exit(0);
}
#endif /* BOARD_NATIVE */

void *lwm2m_connect_server(uint16_t sec_obj_inst_id, void *user_data)
{
    client_data_t *data;
    lwm2m_list_t *instance;
    lwm2m_connection_t *new_conn = NULL;

    data = (client_data_t *)user_data;
    lwm2m_object_t *security_obj = data->security_obj;

    instance = LWM2M_LIST_FIND(data->security_obj->instanceList, sec_obj_inst_id);
    if (instance == NULL) {
        fprintf(stderr, "LWM2M_LIST_FIND() failed\n");
        return NULL;
    }

    new_conn = lwm2m_connection_create(data->conn_list, security_obj, instance->id,
                                       data->lwm2mH);
    if (new_conn == NULL) {
        fprintf(stderr, "Connection creation failed.\n");
        return NULL;
    }

    data->conn_list = new_conn;
    return (void *)new_conn;
}

void lwm2m_close_connection(void *sessionH, void *user_data)
{
    client_data_t *app_data;
    lwm2m_connection_t *target;

    app_data = (client_data_t *)user_data;
    target = (lwm2m_connection_t *)sessionH;

    if (target == app_data->conn_list) {
        app_data->conn_list = target->next;
    }
    else {
        lwm2m_connection_t *parent;

        parent = app_data->conn_list;
        while (parent != NULL && parent->next != target) {
            parent = parent->next;
        }
        if (parent != NULL) {
            parent->next = target->next;
            lwm2m_free(target);
        }
    }
}

#ifdef LWM2M_BOOTSTRAP

static void prv_backup_objects(lwm2m_context_t *context)
{
    uint16_t i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backup_object_array[i]) {
            switch (backup_object_array[i]->objID) {
                case LWM2M_SECURITY_OBJECT_ID:
                    clean_security_object(backup_object_array[i]);
                    lwm2m_free(backup_object_array[i]);
                    break;
                case LWM2M_SERVER_OBJECT_ID:
                    clean_server_object(backup_object_array[i]);
                    lwm2m_free(backup_object_array[i]);
                    break;
                default:
                    break;
            }
        }
        backup_object_array[i] =
            (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backup_object_array[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    copy_security_object(backup_object_array[0],
                         (lwm2m_object_t *)LWM2M_LIST_FIND(
                             context->objectList, LWM2M_SECURITY_OBJECT_ID));
    copy_server_object(backup_object_array[1],
                       (lwm2m_object_t *)LWM2M_LIST_FIND(
                           context->objectList, LWM2M_SERVER_OBJECT_ID));
}

void prv_restore_objects(lwm2m_context_t *context)
{
    lwm2m_object_t *target;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    target = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList,
                                                LWM2M_SECURITY_OBJECT_ID);
    /* first delete internal content */
    clean_security_object(target);
    /* then restore previous object */
    copy_security_object(target, backup_object_array[0]);

    target = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList,
                                                LWM2M_SERVER_OBJECT_ID);
    /* first delete internal content */
    clean_server_object(target);
    /* then restore previous object */
    copy_server_object(target, backup_object_array[1]);

    /* restart the old servers */
    fprintf(stdout, "[BOOTSTRAP] ObjectList restored\r\n");
}

static void update_bootstrap_info(lwm2m_client_state_t *previous_bootstrap_state,
                                  lwm2m_context_t *context)
{
    if (*previous_bootstrap_state != context->state) {
        *previous_bootstrap_state = context->state;
        switch (context->state) {
            case STATE_BOOTSTRAPPING:
#ifdef WITH_LOGS
                fprintf(stdout,
                        "[BOOTSTRAP] backup security and server objects\r\n");
#endif
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}

static void close_backup_object(void)
{
    int i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backup_object_array[i]) {
            switch (backup_object_array[i]->objID) {
                case LWM2M_SECURITY_OBJECT_ID:
                    clean_security_object(backup_object_array[i]);
                    lwm2m_free(backup_object_array[i]);
                    break;
                case LWM2M_SERVER_OBJECT_ID:
                    clean_server_object(backup_object_array[i]);
                    lwm2m_free(backup_object_array[i]);
                    break;
                default:
                    break;
            }
        }
    }
}
#endif

void *lwm2m_run_server(void *arg)
{
    (void)arg;
    client_data_t data;
    int result;
    lwm2m_context_t *lwm2mH = NULL;
    char *server_uri = LWM2M_SERVER_URI;
    int server_id = LWM2M_SERVER_ID;
    int lifetime = LWM2M_DEVICE_TTL;
    time_t reboot_time = 0;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;

#ifdef LWM2M_BOOTSTRAP
    bool bootstrapRequested = LWM2M_SERVER_IS_BOOTSTRAP;
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

#ifdef MODULE_TINYDTLS
    char *psk_id = LWM2M_SERVER_PSK_ID;
    char *psk = LWM2M_SERVER_PSK_KEY;
#else /* MODULE_TINYDTLS */
    char *psk_id = NULL;
#endif /* MODULE_TINYDTLS */
    uint16_t psk_len = -1;
    char *psk_buffer = NULL;

    msg_t _reader_queue[READER_QUEUE_SIZE];
    DEBUG("msg_init_queue()\r\n");
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);

    memset(&data, 0, sizeof(client_data_t));

    local.port = LWM2M_DTLS_PORT;
    if (sock_udp_create(&data.sock, &local, NULL, 0) != 0) {
        fprintf(stderr, "ERROR: Can't create server socket\n");
        return NULL;
    }


    /*
     * Now the main function fill an array with each object, this list will be
     * later
     * passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#ifdef MODULE_TINYDTLS
    if (psk != NULL) {
        psk_len = strlen(psk) / 2;
        psk_buffer = lwm2m_malloc(psk_len);

        if (NULL == psk_buffer) {
            fprintf(stderr, "Failed to create PSK binary buffer\r\n");
            return NULL;
        }
        /* Hex string to binary */
        char *h = psk;
        char *b = psk_buffer;

        for (; *h; h += 2, ++b) {
            char tmp[2];
            tmp[0] = *h;
            tmp[1] = *(h + 1);

            *b = (char)strtol(tmp, NULL, 16);
        }
    }
#endif /* MODULE_TINYDTLS */

    DEBUG("get_security_object(%d, %s, %s, %s, %d, ...)\n", server_id, server_uri,
          psk_id, psk_buffer, psk_len);
#ifdef LWM2M_BOOTSTRAP
    obj_array[0] = get_security_object(server_id, server_uri, psk_id, psk_buffer,
                                       psk_len, bootstrapRequested);
#else
    obj_array[0] = get_security_object(server_id, server_uri, psk_id, psk_buffer,
                                       psk_len, false);
#endif
    if (NULL == obj_array[0]) {
        fprintf(stderr, "Failed to create security object\r\n");
        return NULL;
    }
    data.security_obj = obj_array[0];

    obj_array[1] = get_server_object(server_id, "U", lifetime, false);
    if (NULL == obj_array[1]) {
        fprintf(stderr, "Failed to create server object\r\n");
        return NULL;
    }

    obj_array[2] = lwm2m_get_object_device();
    if (NULL == obj_array[2]) {
        fprintf(stderr, "Failed to create Device object\r\n");
        return NULL;
    }

    int instId = 0;
    obj_array[3] = acc_ctrl_create_object();
    if (NULL == obj_array[3]) {
        fprintf(stderr, "Failed to create Access Control object\r\n");
        return NULL;
    }
    else if (acc_ctrl_obj_add_inst(obj_array[3], instId, 3, 0, server_id) ==
             false) {
        fprintf(stderr, "Failed to create Access Control object instance\r\n");
        return NULL;
    }
    else if (acc_ctrl_oi_add_ac_val(obj_array[3], instId, 0, 15) ==
             false) { /* 0b000000000001111 */
        fprintf(stderr,
                "Failed to create Access Control ACL default resource\r\n");
        return NULL;
    }
    else if (acc_ctrl_oi_add_ac_val(obj_array[3], instId, 999, 1) ==
             false) { /* 0b000000000000001 */
        fprintf(
            stderr,
            "Failed to create Access Control ACL resource for server_id: 999\r\n");
        return NULL;
    }
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    DEBUG("lwm2m_init()\r\n");
    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH) {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return NULL;
    }

    data.lwm2mH = lwm2mH;

    /*
     * We configure the liblwm2m library with the name of the client - which
     * shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    DEBUG("lwm2m_configure()\r\n");
    result = lwm2m_configure(lwm2mH, LWM2M_DEVICE_NAME, NULL, NULL, OBJ_COUNT,
                             obj_array);
    if (result != 0) {
        fprintf(stderr, "lwm2m_configure() failed: 0x%X\r\n", result);
        return NULL;
    }

    /* This will auto-add all supported devices currently in the SAUL registry */
    lwm2m_auto_add_float_sensors(lwm2mH);
    lwm2m_auto_add_bool_sensors(lwm2mH);
    lwm2m_auto_add_bool_actuators(lwm2mH);

#ifdef BOARD_NATIVE
    signal(SIGINT, handle_sigint);
#endif /* BOARD_NATIVE */

    /*
     * We now enter in a while loop that will handle the communications from the
     * server
     */
    while (0 == g_quit) {
        time_t tv;
        uint8_t packet_rcvd[RCVD_SIZE];
        ssize_t pckt_rcvd_size = RCVD_SIZE;
        sock_udp_ep_t remote;

        lwm2m_memarray_print_stats();

        if (lwm2m_device_reboot_requested()) {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time) {
                fprintf(stderr, "reboot requested; rebooting in %u seconds\r\n", REBOOT_TIME);
                reboot_time = tv_sec + REBOOT_TIME;
            }
            if (reboot_time < tv_sec) {
                /*
                 * Message should normally be lost with reboot ...
                 */
                fprintf(stderr, "reboot time expired, rebooting ...");
                pm_reboot();
            }
            else {
                tv = reboot_time - tv_sec;
            }
        }
        else {
            tv = RESPONSE_TIMEOUT;
        }

        /* Update sensor values. */
        lwm2m_poll_float_sensors(lwm2mH);
        lwm2m_poll_bool_sensors(lwm2mH);
        lwm2m_poll_bool_actuators(lwm2mH);

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some
         * packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the
         * state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        DEBUG("lwm2m_step()\n");
        result = lwm2m_step(lwm2mH, &tv);
        fprintf(stdout, " -> State: ");
        switch (lwm2mH->state) {
            case STATE_INITIAL:
                fprintf(stdout, "STATE_INITIAL\r\n");
                break;
            case STATE_BOOTSTRAP_REQUIRED:
                fprintf(stdout, "STATE_BOOTSTRAP_REQUIRED\r\n");
                break;
            case STATE_BOOTSTRAPPING:
                fprintf(stdout, "STATE_BOOTSTRAPPING\r\n");
                break;
            case STATE_REGISTER_REQUIRED:
                fprintf(stdout, "STATE_REGISTER_REQUIRED\r\n");
                break;
            case STATE_REGISTERING:
                fprintf(stdout, "STATE_REGISTERING\r\n");
                break;
            case STATE_READY:
                fprintf(stdout, "STATE_READY\ren");
                break;
            default:
                fprintf(stdout, "Unknown...\r\n");
                break;
        }
        if (result != 0) {
            fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
#ifdef LWM2M_BOOTSTRAP
            if (previousState == STATE_BOOTSTRAPPING) {
#ifdef WITH_LOGS
                fprintf(stdout,
                        "[BOOTSTRAP] restore security and server objects\r\n");
#endif          /* WITH_LOGS */
                prv_restore_objects(lwm2mH);
                lwm2mH->state = STATE_INITIAL;
            }
            else
#endif      /* LWM2M_BOOTSTRAP */
            {
                g_quit = 1;
                continue;
            }
        }
#ifdef LWM2M_BOOTSTRAP
        DEBUG("update_bootstrap_info()\r\n");
        update_bootstrap_info(&previousState, lwm2mH);
#endif

        pckt_rcvd_size = sock_udp_recv(&data.sock, &packet_rcvd, RCVD_SIZE, tv * US_PER_SEC, &remote);
        DEBUG("sock_udp_recv()\r\n");
        if (pckt_rcvd_size > 0) {
            lwm2m_connection_t *conn = lwm2m_connection_find(data.conn_list, &remote);
            if (conn != NULL) {
                DEBUG("lwm2m_connection_handle_packet(%d)\r\n", pckt_rcvd_size);
                int result = lwm2m_connection_handle_packet(conn, packet_rcvd, pckt_rcvd_size);
                if (0 != result) {
                    printf("error handling message %d\n", result);
                }
            }
        }
        else if ((pckt_rcvd_size < 0) &&
                 ((pckt_rcvd_size != -EAGAIN) && (pckt_rcvd_size != -ETIMEDOUT))) {
            DEBUG("Unexpected sock_udp_recv error code %i\n", pckt_rcvd_size);
        }
    }

    /*
     * Finally when the loop is left smoothly - asked by user in the command line
     * interface - we unregister our client from it
     */
    if (psk_buffer != NULL) {
        lwm2m_free(psk_buffer);
    }

#ifdef LWM2M_BOOTSTRAP
    close_backup_object();
#endif
    lwm2m_close(lwm2mH);
    sock_udp_close(&data.sock);
    lwm2m_connection_free(data.conn_list);

    clean_security_object(obj_array[0]);
    lwm2m_free(obj_array[0]);
    clean_server_object(obj_array[1]);
    lwm2m_free(obj_array[1]);
    lwm2m_free_object_device(obj_array[2]);
    acl_ctrl_free_object(obj_array[3]);

    return NULL;
}
