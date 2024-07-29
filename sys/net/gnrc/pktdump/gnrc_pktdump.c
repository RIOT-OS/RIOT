/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_pktdump
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
static msg_t _msg_queue[GNRC_PKTDUMP_MSG_QUEUE_SIZE];

static void _dump_snip(gnrc_pktsnip_t *pkt)
{
    size_t hdr_len = 0;

    switch (pkt->type) {
    case GNRC_NETTYPE_NETIF:
        printf("NETTYPE_NETIF (%i)\n", pkt->type);
        if (IS_USED(MODULE_GNRC_NETIF_HDR)) {
            gnrc_netif_hdr_print(pkt->data);
            hdr_len = pkt->size;
        }
        break;
    case GNRC_NETTYPE_UNDEF:
        printf("NETTYPE_UNDEF (%i)\n", pkt->type);
        break;
#if IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
    case GNRC_NETTYPE_SIXLOWPAN:
        printf("NETTYPE_SIXLOWPAN (%i)\n", pkt->type);
        if (IS_USED(MODULE_SIXLOWPAN)) {
            sixlowpan_print(pkt->data, pkt->size);
            hdr_len = pkt->size;
        }
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN) */
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
    case GNRC_NETTYPE_IPV6:
        printf("NETTYPE_IPV6 (%i)\n", pkt->type);
        if (IS_USED(MODULE_IPV6_HDR)) {
            ipv6_hdr_print(pkt->data);
            hdr_len = sizeof(ipv6_hdr_t);
        }
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_IPV6) */
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT)
    case GNRC_NETTYPE_IPV6_EXT:
        printf("NETTYPE_IPV6_EXT (%i)\n", pkt->type);
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT) */
#if IS_USED(MODULE_GNRC_NETTYPE_ICMPV6)
    case GNRC_NETTYPE_ICMPV6:
        printf("NETTYPE_ICMPV6 (%i)\n", pkt->type);
        if (IS_USED(MODULE_ICMPV6)) {
            icmpv6_hdr_print(pkt->data);
            hdr_len = sizeof(icmpv6_hdr_t);
        }
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_ICMPV6) */
#if IS_USED(MODULE_GNRC_NETTYPE_CCN)
    case GNRC_NETTYPE_CCN_CHUNK:
        printf("GNRC_NETTYPE_CCN_CHUNK (%i)\n", pkt->type);
        printf("Content is: %.*s\n", (int)pkt->size, (char*)pkt->data);
        hdr_len = pkt->size;
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_CCN) */
#if IS_USED(MODULE_GNRC_NETTYPE_NDN)
    case GNRC_NETTYPE_NDN:
            printf("NETTYPE_NDN (%i)\n", pkt->type);
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_NDN) */
#if IS_USED(MODULE_GNRC_NETTYPE_TCP)
    case GNRC_NETTYPE_TCP:
        printf("NETTYPE_TCP (%i)\n", pkt->type);
        if (IS_USED(MODULE_TCP)) {
            tcp_hdr_print(pkt->data);
            hdr_len = sizeof(tcp_hdr_t);
        }
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_TCP) */
#if IS_USED(MODULE_GNRC_NETTYPE_UDP)
    case GNRC_NETTYPE_UDP:
        printf("NETTYPE_UDP (%i)\n", pkt->type);
        if (IS_USED(MODULE_UDP)) {
            udp_hdr_print(pkt->data);
            hdr_len = sizeof(udp_hdr_t);
        }
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_UDP) */
#ifdef TEST_SUITES
    case GNRC_NETTYPE_TEST:
        printf("NETTYPE_TEST (%i)\n", pkt->type);
        break;
#endif
    default:
        printf("NETTYPE_UNKNOWN (%i)\n", pkt->type);
        break;
    }
    if (hdr_len < pkt->size) {
        size_t size = pkt->size - hdr_len;

        od_hex_dump(((uint8_t *)pkt->data) + hdr_len, size, OD_WIDTH_DEFAULT);
    }
}

static void _dump(gnrc_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    gnrc_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3" PRIuSIZE " byte, type: ", snips,
               snip->size);
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

    /* setup the message queue */
    msg_init_queue(_msg_queue, GNRC_PKTDUMP_MSG_QUEUE_SIZE);

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
                             0,
                             _eventloop, NULL, "pktdump");
    }
    return gnrc_pktdump_pid;
}
