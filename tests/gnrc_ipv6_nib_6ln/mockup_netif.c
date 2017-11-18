/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "common.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netdev.h"
#include "sched.h"
#include "thread.h"

#define _MSG_QUEUE_SIZE  (2)

kernel_pid_t _mock_netif_pid = KERNEL_PID_UNDEF;

static char _mock_netif_stack[THREAD_STACKSIZE_DEFAULT];
static gnrc_netreg_entry_t dumper;
static msg_t _main_msg_queue[_MSG_QUEUE_SIZE];
static msg_t _mock_netif_msg_queue[_MSG_QUEUE_SIZE];

static void *_mock_netif_thread(void *args)
{
    msg_t msg, reply = { .type = GNRC_NETAPI_MSG_TYPE_ACK };

    (void)args;
    msg_init_queue(_mock_netif_msg_queue, _MSG_QUEUE_SIZE);
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_GET:
                reply.content.value = (uint32_t)_mock_netif_get(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                reply.content.value = (uint32_t)(-ENOTSUP);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
            case GNRC_NETAPI_MSG_TYPE_RCV:
                gnrc_pktbuf_release(msg.content.ptr);
        }
        msg_reply(&msg, &reply);
    }
    return NULL;
}

void _common_set_up(void)
{
    gnrc_ipv6_nib_init();
    gnrc_ipv6_nib_init_iface(_mock_netif_pid);
}

void _tests_init(void)
{
    msg_init_queue(_main_msg_queue, _MSG_QUEUE_SIZE);
    _mock_netif_pid = thread_create(_mock_netif_stack,
                                    sizeof(_mock_netif_stack),
                                    GNRC_NETDEV_MAC_PRIO,
                                    THREAD_CREATE_STACKTEST,
                                    _mock_netif_thread, NULL, "mock_netif");
    assert(_mock_netif_pid > KERNEL_PID_UNDEF);
    gnrc_netif_add(_mock_netif_pid);
    gnrc_ipv6_netif_init_by_dev();
    thread_yield();
    gnrc_netreg_entry_init_pid(&dumper, GNRC_NETREG_DEMUX_CTX_ALL,
                               sched_active_pid);
    gnrc_netreg_register(GNRC_NETTYPE_NDP2, &dumper);
}

/** @} */
