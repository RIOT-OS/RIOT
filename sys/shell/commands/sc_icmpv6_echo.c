/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#ifdef MODULE_GNRC_ICMPV6

#include "byteorder.h"
#include "net/gnrc/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc.h"
#include "thread.h"
#include "utlist.h"
#include "xtimer.h"

static uint16_t id = 0x53;
static uint16_t min_seq_expected = 0;
static uint16_t max_seq_expected = 0;
static char ipv6_str[IPV6_ADDR_MAX_STR_LEN];

static void usage(char **argv)
{
    printf("%s [<count>] <ipv6 addr> [<payload_len>] [<delay in ms>] [<stats interval>]\n", argv[0]);
    puts("defaults:");
    puts("    count = 3");
    puts("    payload_len = 4");
    puts("    delay = 1000");
    puts("    stats interval = count");
}

void _set_payload(icmpv6_echo_t *hdr, size_t payload_len)
{
    size_t i = 0;
    uint8_t *payload = (uint8_t *)(hdr + 1);

    while (i < payload_len) {
        if (id > 0xff) {
            payload[i] = (uint8_t)(id >> 8);
            payload[i + 1] = (uint8_t)(id & 0xff);
            i += 2;
        }
        else {
            payload[i++] = (uint8_t)(id & 0xff);
        }
    }

    if (i > payload_len) {  /* when id > 0xff and payload_len is odd */
        payload[payload_len - 1] = id >> 8;
    }
}

static inline bool _expected_seq(uint16_t seq)
{
    /* take integer overflows in account */
    if (min_seq_expected > max_seq_expected) {
        return (seq >= min_seq_expected) || (seq <= max_seq_expected);
    }
    else {
        return (seq >= min_seq_expected) && (seq <= max_seq_expected);
    }
}

int _handle_reply(gnrc_pktsnip_t *pkt, uint32_t time)
{
    gnrc_pktsnip_t *ipv6, *icmpv6;
    ipv6_hdr_t *ipv6_hdr;
    icmpv6_echo_t *icmpv6_hdr;
    uint16_t seq;

    LL_SEARCH_SCALAR(pkt, ipv6, type, GNRC_NETTYPE_IPV6);
    LL_SEARCH_SCALAR(pkt, icmpv6, type, GNRC_NETTYPE_ICMPV6);

    if ((ipv6 == NULL) || (icmpv6 == NULL)) {
        puts("error: IPv6 header or ICMPv6 header not found in reply");
        return 0;
    }

    ipv6_hdr = ipv6->data;
    icmpv6_hdr = icmpv6->data;
    seq = byteorder_ntohs(icmpv6_hdr->seq);

    if ((byteorder_ntohs(icmpv6_hdr->id) == id) && _expected_seq(seq)) {
        if (seq <= min_seq_expected) {
            min_seq_expected++;
        }

        printf("%u bytes from %s: id=%" PRIu16 " seq=%" PRIu16 " hop limit=%" PRIu8
               " time = %" PRIu32 ".%03" PRIu32 " ms\n", (unsigned) icmpv6->size,
               ipv6_addr_to_str(ipv6_str, &(ipv6_hdr->src), sizeof(ipv6_str)),
               byteorder_ntohs(icmpv6_hdr->id), seq, ipv6_hdr->hl,
               time / MS_IN_USEC, time % MS_IN_USEC);
        gnrc_ipv6_nc_still_reachable(&ipv6_hdr->src);
    }
    else {
        puts("error: unexpected parameters");
        return 0;
    }

    return 1;
}

static void _print_stats(char *addr_str, int success, int count, uint64_t total_time,
                         uint64_t sum_rtt, uint32_t min_rtt, uint32_t max_rtt)
{
    printf("--- %s ping statistics ---\n", addr_str);

    if (success > 0) {
        uint32_t avg_rtt = (uint32_t)sum_rtt / count;  /* get average */
        printf("%d packets transmitted, %d received, %d%% packet loss, time %"
               PRIu32 ".06%" PRIu32 " s\n", count, success,
               (100 - ((success * 100) / count)),
               (uint32_t)total_time / SEC_IN_USEC, (uint32_t)total_time % SEC_IN_USEC);
        printf("rtt min/avg/max = "
               "%" PRIu32 ".%03" PRIu32 "/"
               "%" PRIu32 ".%03" PRIu32 "/"
               "%" PRIu32 ".%03" PRIu32 " ms\n",
               min_rtt / MS_IN_USEC, min_rtt % MS_IN_USEC,
               avg_rtt / MS_IN_USEC, avg_rtt % MS_IN_USEC,
               max_rtt / MS_IN_USEC, max_rtt % MS_IN_USEC);
    }
    else {
        printf("%d packets transmitted, 0 received, 100%% packet loss\n", count);
    }
}

int _icmpv6_ping(int argc, char **argv)
{
    int count = 3, success = 0, remaining, stat_interval = 0, stat_counter = 0;
    size_t payload_len = 4;
    uint32_t delay = 1 * SEC_IN_MS;
    char *addr_str;
    ipv6_addr_t addr;
    msg_t msg;
    gnrc_netreg_entry_t *ipv6_entry, my_entry = { NULL, ICMPV6_ECHO_REP,
                                                  thread_getpid()
                                                };
    uint32_t min_rtt = UINT32_MAX, max_rtt = 0;
    uint64_t sum_rtt = 0;
    uint64_t ping_start;
    int param_offset = 0;

    if (argc < 2) {
        usage(argv);
        return 1;
    }
    else if ((argc > 2) &&  ((count = atoi(argv[1])) > 0)) {
        param_offset = 1;
    }
    else {
        count = 3;
    }

    stat_interval = count;

    addr_str = argv[1 + param_offset];

    if (argc > (2 + param_offset)) {
        payload_len = atoi(argv[2 + param_offset]);
    }
    if (argc > (3 + param_offset)) {
        delay = atoi(argv[3 + param_offset]);
    }
    if (argc > (4 + param_offset)) {
        stat_interval = atoi(argv[4 + param_offset]);
    }

    if ((ipv6_addr_from_str(&addr, addr_str) == NULL) || (((int)payload_len) < 0)) {
        usage(argv);
        return 1;
    }

    if (gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &my_entry) < 0) {
        puts("error: network registry is full");
        return 1;
    }

    ipv6_entry = gnrc_netreg_lookup(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL);

    if (ipv6_entry == NULL) {
        puts("error: ipv6 thread missing");
        return 1;
    }

    remaining = count;

    ping_start = xtimer_now64();

    while ((remaining--) > 0) {
        gnrc_pktsnip_t *pkt;
        uint32_t start, stop, timeout = 1 * SEC_IN_USEC;

        pkt = gnrc_icmpv6_echo_build(ICMPV6_ECHO_REQ, id, ++max_seq_expected,
                                     NULL, payload_len);

        if (pkt == NULL) {
            puts("error: packet buffer full");
            continue;
        }

        _set_payload(pkt->data, payload_len);

        pkt = gnrc_netreg_hdr_build(GNRC_NETTYPE_IPV6, pkt, NULL, 0, addr.u8,
                                    sizeof(ipv6_addr_t));

        if (pkt == NULL) {
            puts("error: packet buffer full");
            continue;
        }

        start = xtimer_now();
        if (gnrc_netapi_send(ipv6_entry->pid, pkt) < 1) {
            puts("error: unable to send ICMPv6 echo request\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        /* TODO: replace when #4219 was fixed */
        if (xtimer_msg_receive_timeout64(&msg, (uint64_t)timeout) >= 0) {
            switch (msg.type) {
                case GNRC_NETAPI_MSG_TYPE_RCV:
                    stop = xtimer_now() - start;

                    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *)msg.content.ptr;
                    success += _handle_reply(pkt, stop);
                    gnrc_pktbuf_release(pkt);

                    if (stop > max_rtt) {
                        max_rtt = stop;
                    }

                    if (stop < min_rtt) {
                        min_rtt = stop;
                    }

                    sum_rtt += stop;

                    break;

                default:
                    /* requeue wrong packets */
                    msg_send(&msg, thread_getpid());
                    break;
            }
        }
        else {
            puts("ping timeout");
        }

        while (msg_try_receive(&msg) > 0) {
            if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
                printf("dropping additional response packet (probably caused by duplicates)\n");
                gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *)msg.content.ptr;
                gnrc_pktbuf_release(pkt);
            }
        }

        if (remaining > 0) {
            xtimer_usleep64(delay * MS_IN_USEC);
        }
        if ((++stat_counter == stat_interval) || (remaining == 0)) {
            uint64_t total_time = xtimer_now64() - ping_start;
            _print_stats(addr_str, success, (count - remaining), total_time, sum_rtt, min_rtt,
                         max_rtt);
            stat_counter = 0;
        }

    }

    max_seq_expected = 0;
    id++;

    gnrc_netreg_unregister(GNRC_NETTYPE_ICMPV6, &my_entry);
    while (msg_try_receive(&msg) > 0) {
        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            printf("dropping additional response packet (probably caused by duplicates)\n");
            gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *)msg.content.ptr;
            gnrc_pktbuf_release(pkt);
        }
    }

    return success ? 0 : 1;
}

#endif

/**
 * @}
 */
