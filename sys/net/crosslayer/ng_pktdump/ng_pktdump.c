/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_pktdump
 * @{
 *
 * @file
 * @brief       Generic module to dump packages received via netapi to STDOUT
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "kernel.h"
#include "net/ng_pktdump.h"
#include "net/ng_netbase.h"

/**
 * @brief   PID of the pktdump thread
 */
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[NG_PKTDUMP_STACKSIZE];

void _dump_type(ng_nettype_t type)
{
    switch (type) {
        case NG_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)", type);
            break;

#ifdef MODULE_NG_SIXLOWPAN
        case NG_NETTYPE_SIXLOWPAN:
            printf("NETTYPE_SIXLOPAN (%i)", type);
            break;
#endif
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            printf("NETTYPE_IPV6 (%i)", type);
            break;
#endif
#ifdef MODULE_NG_ICMPV6
        case NG_NETTYPE_ICMPV6:
            printf("NETTYPE_ICMPV6 (%i)", type);
            break;
#endif
#ifdef MODULE_NG_TCP
        case NG_NETTYPE_TCP:
            printf("NETTYPE_TCP (%i)", type);
            break;
#endif
#ifdef MODULE_NG_UDP
        case NG_NETTYPE_UDP:
            printf("NETTYPE_UDP (%i)", type);
            break;
#endif
#ifdef TEST_SUITES
        case NG_NETTYPE_TEST:
            printf("NETTYPE_TEST (%i)", type);
            break;
#endif
        default:
            printf("NETTYPE_UNKNOWN (%i)", type);
            break;
    }
}

void _dump(ng_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    ng_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3i byte, type: ", snips, snip->size);
        _dump_type(snip->type);
        puts("");
        ++snips;
        size += snip->size;
        snip = snip->next;
    }
    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    ng_pktbuf_release(pkt);
}

void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[NG_PKTDUMP_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, NG_PKTDUMP_MSG_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:");
                _dump((ng_pktsnip_t *)msg.content.ptr);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                _dump((ng_pktsnip_t *)msg.content.ptr);
                break;
            default:
                puts("PKTDUMP: received something unexpected");
                break;
        }
    }

    /* never reached */
    return NULL;
}

kernel_pid_t ng_pktdump_getpid(void)
{
    return _pid;
}

kernel_pid_t ng_pktdump_init(void)
{
    if (_pid == KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, sizeof(_stack), NG_PKTDUMP_PRIO,
                             CREATE_STACKTEST, _eventloop, NULL, "pktdump");
    }
    return _pid;
}
