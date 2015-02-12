/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup sys_net_ng_lomac
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>

#include "cpu-conf.h"
#include "kernel_types.h"
#include "msg.h"
#include "net/ng_lomac.h"
#include "net/ng_netapi.h"
#include "net/ng_netif.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _LOMAC_STACKSIZE    (KERNEL_CONF_STACKSIZE_IDLE)

static kernel_pid_t _lomac_pid = KERNEL_PID_UNDEF;
static char _lomac_stack[_LOMAC_STACKSIZE];
static msg_t _msg_queue[NG_LOMAC_MSG_QUEUE_SIZE];

static void *_lomac_thread(void *args)
{
    msg_t msg, reply;

    (void)args;
    msg_init_queue(_msg_queue, NG_LOMAC_MSG_QUEUE_SIZE);

    /* register as interface */
    ng_netif_add(thread_getpid());

    /* preinitialize ACK as "I don't support anything" */
    reply.type = NG_NETAPI_MSG_TYPE_ACK;
    reply.content.value = (uint32_t)(-ENOTSUP);

    /* start event loop */
    while (1) {
        DEBUG("lomac: waiting for incoming message.\n");
        msg_receive(&msg);

        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_SND:
                DEBUG("lomac: NG_NETDEV_MSG_TYPE_SND received\n");
                msg_send(&msg, msg.sender_pid);
                break;

            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;

            default:
                break;
        }
    }

    return NULL;
}

kernel_pid_t ng_lomac_init(char priority)
{
    if (_lomac_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }

    _lomac_pid = thread_create(_lomac_stack, _LOMAC_STACKSIZE, priority,
                               CREATE_STACKTEST, _lomac_thread, NULL, "ng_lomac");

    if (_lomac_pid <= 0) {
        kernel_pid_t res = _lomac_pid;
        _lomac_pid = KERNEL_PID_UNDEF;
        return res;
    }

    return _lomac_pid;
}

/**
 * @}
 */
