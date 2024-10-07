/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Forward Proxy Thread
 *
 * @author  Mariem Charrada <mariem.charrada@ml-pa.com>
 * @}
 */

#include "msg.h"
#include "net/gcoap.h"
#include "net/gcoap/forward_proxy.h"
#include "thread.h"

#include "forward_proxy_internal.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static char _forward_proxy_thread[GCOAP_PROXY_STACK_SIZE];
kernel_pid_t forward_proxy_pid = KERNEL_PID_UNDEF;

static void *_forward_proxy_thread_start(void *arg)
{
    (void)arg;

    msg_t _forward_proxy_msg_queue[CONFIG_GCOAP_REQ_WAITING_MAX];
    msg_init_queue(_forward_proxy_msg_queue, ARRAY_SIZE(_forward_proxy_msg_queue));

    msg_t msg;

    while (1) {
        msg_receive(&msg);
        client_ep_t *cep = (client_ep_t *)msg.content.ptr;
        switch (msg.type) {
            case GCOAP_FORWARD_PROXY_MSG_SEND: {
                gcoap_forward_proxy_req_send(cep);
                break;
            }
            default:
                DEBUG_PUTS("_forward_proxy_thread_start: unknown message type\n");
                break;
        }
    }
    return NULL;
}

void gcoap_forward_proxy_thread_init(void)
{
    forward_proxy_pid = thread_create(_forward_proxy_thread, sizeof(_forward_proxy_thread),
                                       THREAD_PRIORITY_MAIN - 1, 0,
                                       _forward_proxy_thread_start, NULL, "gcoap proxy");
    if (forward_proxy_pid <= KERNEL_PID_UNDEF) {
        DEBUG_PUTS("gcoap_forward_proxy_thread_init(): thread_create failed\n");
    }
}
