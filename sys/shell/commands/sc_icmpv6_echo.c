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
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/nc.h"
#endif
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/netif.h"
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
    printf("%s [<count>] <ipv6 addr>[%%<interface>] [<payload_len>] [<delay in ms>] [<stats interval>]\n", argv[0]);
    puts("defaults:");
    puts("    count = 3");
    puts("    interface = first interface if only one present, only needed for link-local addresses");
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

    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    icmpv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV6);

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

        printf("%u bytes from %s: id=%" PRIu16 " seq=%" PRIu16 " hop limit=%u time = %"
               PRIu32 ".%03" PRIu32 " ms\n", (unsigned) icmpv6->size,
               ipv6_addr_to_str(ipv6_str, &(ipv6_hdr->src), sizeof(ipv6_str)),
               byteorder_ntohs(icmpv6_hdr->id), seq, (unsigned)ipv6_hdr->hl,
               time / US_PER_MS, time % US_PER_MS);
#ifdef MODULE_GNRC_IPV6_NIB
        gnrc_ipv6_nib_nc_mark_reachable(&ipv6_hdr->src);
#endif
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
        uint32_t avg_rtt = (uint32_t)(sum_rtt / success); /* get average */
        printf("%d packets transmitted, %d received, %d%% packet loss, time %"
               PRIu32 ".06%" PRIu32 " s\n", count, success,
               (100 - ((success * 100) / count)),
               (uint32_t)total_time / US_PER_SEC, (uint32_t)total_time % US_PER_SEC);
        printf("rtt min/avg/max = "
               "%" PRIu32 ".%03" PRIu32 "/"
               "%" PRIu32 ".%03" PRIu32 "/"
               "%" PRIu32 ".%03" PRIu32 " ms\n",
               min_rtt / US_PER_MS, min_rtt % US_PER_MS,
               avg_rtt / US_PER_MS, avg_rtt % US_PER_MS,
               max_rtt / US_PER_MS, max_rtt % US_PER_MS);
    }
    else {
        printf("%d packets transmitted, 0 received, 100%% packet loss\n", count);
    }
}

int _icmpv6_ping(int argc, char **argv)
{
    int count = 3, success = 0, remaining, stat_interval = 0, stat_counter = 0;
    size_t payload_len = 4;
    uint32_t delay = 1 * MS_PER_SEC;
    char *addr_str;
    ipv6_addr_t addr;
    kernel_pid_t src_iface;
    msg_t msg;
    gnrc_netreg_entry_t my_entry = GNRC_NETREG_ENTRY_INIT_PID(ICMPV6_ECHO_REP,
                                                              sched_active_pid);
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

    if ((int)payload_len < 0) {
        usage(argv);
        return 1;
    }

    src_iface = ipv6_addr_split_iface(addr_str);
    if (src_iface == -1) {
        src_iface = KERNEL_PID_UNDEF;
    }

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("error: malformed address");
        return 1;
    }

    if (ipv6_addr_is_link_local(&addr) || (src_iface != KERNEL_PID_UNDEF)) {
        size_t ifnum = gnrc_netif_numof();

        if (src_iface == KERNEL_PID_UNDEF) {
            if (ifnum == 1) {
                src_iface = gnrc_netif_iter(NULL)->pid;
            }
            else {
                puts("error: link local target needs interface parameter (use \"<address>%<ifnum>\")\n");
                return 1;
            }
        }
        else {
            if (gnrc_netif_get_by_pid(src_iface) == NULL) {
                printf("error: %"PRIkernel_pid" is not a valid interface.\n", src_iface);
                return 1;
            }
        }
    }

    if (gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &my_entry) < 0) {
        puts("error: network registry is full");
        return 1;
    }

    remaining = count;

    ping_start = xtimer_now_usec64();

    while ((remaining--) > 0) {
        gnrc_pktsnip_t *pkt;
        uint32_t start, timeout = 1 * US_PER_SEC;

        pkt = gnrc_icmpv6_echo_build(ICMPV6_ECHO_REQ, id, ++max_seq_expected,
                                     NULL, payload_len);

        if (pkt == NULL) {
            puts("error: packet buffer full");
            continue;
        }

        _set_payload(pkt->data, payload_len);

        pkt = gnrc_ipv6_hdr_build(pkt, NULL, &addr);

        if (pkt == NULL) {
            puts("error: packet buffer full");
            continue;
        }

        if (src_iface != KERNEL_PID_UNDEF) {
            pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t),
                                  GNRC_NETTYPE_NETIF);

            if (pkt == NULL) {
                puts("error: packet buffer full");
                continue;
            }

            gnrc_netif_hdr_init(((gnrc_netif_hdr_t *)pkt->data), 0, 0);
            ((gnrc_netif_hdr_t *)pkt->data)->if_pid = src_iface;
        }

        start = xtimer_now_usec();
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
            puts("error: unable to send ICMPv6 echo request\n");
            gnrc_pktbuf_release(pkt);
            continue;
        }

        /* TODO: replace when #4219 was fixed */
        if (xtimer_msg_receive_timeout64(&msg, (uint64_t)timeout) >= 0) {
            uint32_t stop;
            switch (msg.type) {
                case GNRC_NETAPI_MSG_TYPE_RCV:
                    stop = xtimer_now_usec() - start;

                    gnrc_pktsnip_t *pkt = msg.content.ptr;
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
                gnrc_pktsnip_t *pkt = msg.content.ptr;
                gnrc_pktbuf_release(pkt);
            }
        }

        if (remaining > 0) {
            xtimer_usleep64(delay * US_PER_MS);
        }
        if ((++stat_counter == stat_interval) || (remaining == 0)) {
            uint64_t total_time = xtimer_now_usec64() - ping_start;
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
            gnrc_pktsnip_t *pkt = msg.content.ptr;
            gnrc_pktbuf_release(pkt);
        }
    }

    return success ? 0 : 1;
}

#endif

/**
 * @}
 */
