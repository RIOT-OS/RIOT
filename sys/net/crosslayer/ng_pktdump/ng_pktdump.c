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

#include "byteorder.h"
#include "thread.h"
#include "msg.h"
#include "kernel.h"
#include "net/ng_pktdump.h"
#include "net/ng_netbase.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/hdr.h"
#include "od.h"

/**
 * @brief   PID of the pktdump thread
 */
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[NG_PKTDUMP_STACKSIZE];

#define ADDR_STR_MAX    (24)

#ifdef MODULE_NG_NETIF
static void _dump_netif_hdr(ng_netif_hdr_t *hdr)
{
    char addr_str[ADDR_STR_MAX];

    printf("if_pid: %" PRIkernel_pid "  ", hdr->if_pid);
    printf("rssi: %" PRIu8 "  ", hdr->rssi);
    printf("lqi: %" PRIu8 "\n", hdr->lqi);
    printf("src_l2addr: %s\n",
           ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                ng_netif_hdr_get_src_addr(hdr),
                                (size_t)hdr->src_l2addr_len));
    printf("dst_l2addr: %s\n",
           ng_netif_addr_to_str(addr_str, sizeof(addr_str),
                                ng_netif_hdr_get_dst_addr(hdr),
                                (size_t)hdr->dst_l2addr_len));
}
#endif

#ifdef MODULE_NG_IPV6
static void _dump_ipv6_hdr(ng_ipv6_hdr_t *hdr)
{
    char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];

    if (!ng_ipv6_hdr_is(hdr)) {
        printf("illegal version field: %" PRIu8 "\n", ng_ipv6_hdr_get_version(hdr));
    }

    printf("traffic class: 0x%02" PRIx8 " (ECN: 0x%" PRIx8 ", DSCP: 0x%02" PRIx8 ")\n",
           ng_ipv6_hdr_get_tc(hdr), ng_ipv6_hdr_get_tc_ecn(hdr),
           ng_ipv6_hdr_get_tc_dscp(hdr));
    printf("flow label: 0x%05" PRIx32 "\n", ng_ipv6_hdr_get_fl(hdr));
    printf("length: %" PRIu16 "  next header: %" PRIu8 "  hop limit: %" PRIu8 "\n",
           byteorder_ntohs(hdr->len), hdr->nh, hdr->hl);
    printf("source address: %s\n", ng_ipv6_addr_to_str(addr_str, &hdr->src,
            sizeof(addr_str)));
    printf("destination address: %s\n", ng_ipv6_addr_to_str(addr_str, &hdr->dst,
            sizeof(addr_str)));

}
#endif

static void _dump_snip(ng_pktsnip_t *pkt)
{
    switch (pkt->type) {
        case NG_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
#ifdef MODULE_NG_NETIF
        case NG_NETTYPE_NETIF:
            printf("NETTYPE_NETIF (%i)\n", pkt->type);
            _dump_netif_hdr(pkt->data);
            break;
#endif
#ifdef MODULE_NG_SIXLOWPAN
        case NG_NETTYPE_SIXLOWPAN:
            printf("NETTYPE_SIXLOWPAN (%i)\n", pkt->type);
            break;
#endif
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            printf("NETTYPE_IPV6 (%i)\n", pkt->type);
            _dump_ipv6_hdr(pkt->data);
            break;
#endif
#ifdef MODULE_NG_ICMPV6
        case NG_NETTYPE_ICMPV6:
            printf("NETTYPE_ICMPV6 (%i)\n", pkt->type);
            break;
#endif
#ifdef MODULE_NG_TCP
        case NG_NETTYPE_TCP:
            printf("NETTYPE_TCP (%i)\n", pkt->type);
            break;
#endif
#ifdef MODULE_NG_UDP
        case NG_NETTYPE_UDP:
            printf("NETTYPE_UDP (%i)\n", pkt->type);
            break;
#endif
#ifdef TEST_SUITES
        case NG_NETTYPE_TEST:
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

static void _dump(ng_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    ng_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3i byte, type: ", snips, snip->size);
        _dump_snip(snip);
        ++snips;
        size += snip->size;
        snip = snip->next;
    }

    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    ng_pktbuf_release(pkt);
}

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[NG_PKTDUMP_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, NG_PKTDUMP_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

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
            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
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
