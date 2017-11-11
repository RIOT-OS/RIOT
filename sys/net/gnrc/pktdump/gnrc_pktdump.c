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

#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "msg.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "net/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/sixlowpan.h"
#include "od.h"

/**
 * @brief   PID of the pktdump thread
 */
kernel_pid_t gnrc_pktdump_pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[GNRC_PKTDUMP_STACKSIZE];

static void _dump_snip(gnrc_pktsnip_t *pkt)
{
    switch (pkt->type) {
        case GNRC_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
#ifdef MODULE_GNRC_NETIF
        case GNRC_NETTYPE_NETIF:
            printf("NETTYPE_NETIF (%i)\n", pkt->type);
            gnrc_netif_hdr_print(pkt->data);
            break;
#endif
#ifdef MODULE_GNRC_SIXLOWPAN
        case GNRC_NETTYPE_SIXLOWPAN:
            printf("NETTYPE_SIXLOWPAN (%i)\n", pkt->type);
            sixlowpan_print(pkt->data, pkt->size);
            break;
#endif
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            printf("NETTYPE_IPV6 (%i)\n", pkt->type);
            ipv6_hdr_print(pkt->data);
            break;
#endif
#ifdef MODULE_GNRC_ICMPV6
        case GNRC_NETTYPE_ICMPV6:
            printf("NETTYPE_ICMPV6 (%i)\n", pkt->type);
            icmpv6_hdr_print(pkt->data);
            break;
#endif
#ifdef MODULE_GNRC_TCP
        case GNRC_NETTYPE_TCP:
            printf("NETTYPE_TCP (%i)\n", pkt->type);
            tcp_hdr_print(pkt->data);
            break;
#endif
#ifdef MODULE_GNRC_UDP
        case GNRC_NETTYPE_UDP:
            printf("NETTYPE_UDP (%i)\n", pkt->type);
            udp_hdr_print(pkt->data);
            break;
#endif
#ifdef TEST_SUITES
        case GNRC_NETTYPE_TEST:
            printf("NETTYPE_TEST (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
#endif
        default:
            printf("NETTYPE_UNKNOWN (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
    }
}

static void _dump(gnrc_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    gnrc_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3u byte, type: ", snips,
               (unsigned int)snip->size);
        _dump_snip(snip);
        ++snips;
        size += snip->size;
        snip = snip->next;
    }

    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    gnrc_pktbuf_release(pkt);
}

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[GNRC_PKTDUMP_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, GNRC_PKTDUMP_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:");
                _dump(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                _dump(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                puts("PKTDUMP: received something unexpected");
                break;
        }
    }

    /* never reached */
    return NULL;
}

kernel_pid_t gnrc_pktdump_init(void)
{
    if (gnrc_pktdump_pid == KERNEL_PID_UNDEF) {
        gnrc_pktdump_pid = thread_create(_stack, sizeof(_stack), GNRC_PKTDUMP_PRIO,
                             THREAD_CREATE_STACKTEST,
                             _eventloop, NULL, "pktdump");
    }
    return gnrc_pktdump_pid;
}
