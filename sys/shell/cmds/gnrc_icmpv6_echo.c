/*
 * Copyright (C) 2018 Freie Universität Berlin
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
 *
 * This implementation oriented itself on the [version by Mike
 * Muuss](http://ftp.arl.army.mil/~mike/ping.html) which was published under
 * public domain. The state-handling and duplicate detection was inspired by the
 * ping version of [inetutils](://www.gnu.org/software/inetutils/), which was
 * published under GPLv3
 */

#ifdef MODULE_GNRC_ICMPV6
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bitfield.h"
#include "byteorder.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/icmpv6.h"
#include "net/icmpv6.h"
#include "net/ipv6.h"
#include "net/utils.h"
#include "sched.h"
#include "shell.h"
#include "timex.h"
#include "unaligned.h"
#include "utlist.h"
#include "ztimer.h"

#ifdef MODULE_LUID
#include "luid.h"
#endif
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/nc.h"
#endif

#define _SEND_NEXT_PING         (0xEF48)
#define _PING_FINISH            (0xEF49)

#define CKTAB_SIZE              (64U * 8)   /* 64 byte * 8 bit/byte */

#define DEFAULT_COUNT           (3U)
#define DEFAULT_DATALEN         (sizeof(uint32_t))
#define DEFAULT_ID              (0x53)
#define DEFAULT_INTERVAL_USEC   (1U * US_PER_SEC)
#define DEFAULT_TIMEOUT_USEC    (1U * US_PER_SEC)

typedef struct {
    gnrc_netreg_entry_t netreg;
    ztimer_t sched_timer;
    msg_t sched_msg;
    ipv6_addr_t host;
    char *hostname;
    unsigned long num_sent, num_recv, num_rept;
    unsigned long long tsum;
    unsigned tmin, tmax;
    unsigned count;
    size_t datalen;
    BITFIELD(cktab, CKTAB_SIZE);
    uint32_t timeout;
    uint32_t interval;
    gnrc_netif_t *netif;
    uint16_t id;
    uint8_t hoplimit;
} _ping_data_t;

static void _usage(char *cmdname);
static int _configure(int argc, char **argv, _ping_data_t *data);
static void _pinger(_ping_data_t *data);
static int _print_reply(gnrc_pktsnip_t *pkt, int corrupted, uint32_t rtt, void *ctx);
static int _finish(_ping_data_t *data);

static int _gnrc_icmpv6_ping(int argc, char **argv)
{
    _ping_data_t data = {
        .netreg = GNRC_NETREG_ENTRY_INIT_PID(ICMPV6_ECHO_REP,
                                                 thread_getpid()),
        .count = DEFAULT_COUNT,
        .tmin = UINT_MAX,
        .datalen = DEFAULT_DATALEN,
        .timeout = DEFAULT_TIMEOUT_USEC,
        .interval = DEFAULT_INTERVAL_USEC,
        .id = DEFAULT_ID,
    };
    int res;

    ztimer_acquire(ZTIMER_USEC);

    if ((res = _configure(argc, argv, &data)) != 0) {
        goto ret;
    }
    gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &data.netreg);
    _pinger(&data);
    do {
        msg_t msg;

        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV: {
                gnrc_icmpv6_echo_rsp_handle(msg.content.ptr, data.datalen,
                                            _print_reply, &data);
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            }
            case _SEND_NEXT_PING:
                _pinger(&data);
                break;
            case _PING_FINISH:
                goto finish;
            default:
                /* requeue wrong packets */
                msg_send(&msg, thread_getpid());
                break;
        }
    } while (data.num_recv < data.count);
finish:
    ztimer_remove(ZTIMER_USEC, &data.sched_timer);
    res = _finish(&data);
    gnrc_netreg_unregister(GNRC_NETTYPE_ICMPV6, &data.netreg);
    while (msg_avail() > 0) {
        msg_t msg;

        /* remove all remaining messages (likely caused by duplicates) */
        if ((msg_try_receive(&msg) > 0) &&
            (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) &&
            (((gnrc_pktsnip_t *)msg.content.ptr)->type == GNRC_NETTYPE_ICMPV6)) {
            gnrc_pktbuf_release(msg.content.ptr);
        }
        else {
            /* requeue other packets */
            msg_send(&msg, thread_getpid());
        }
    }
ret:
    ztimer_release(ZTIMER_USEC);
    return res;
}

SHELL_COMMAND(ping, "Ping via ICMPv6", _gnrc_icmpv6_ping);

static void _usage(char *cmdname)
{
    printf("%s [-c <count>] [-h] [-i <ms interval>] [-s <packetsize>]\n",
           cmdname);
    printf("     [-t hoplimit] [-W <ms timeout>] <host>[%%<interface>]\n");
    printf("     count: number of pings (default: 3)\n");
    printf("     ms interval: wait interval milliseconds between sending "
              "(default: 1000)\"");
    printf("     packetsize: number of bytes in echo payload; must be >= 4 to "
              "measure round trip time (default: 4)\n");
    printf("     hoplimit: Set the IP time to life/hoplimit "
              "(default: interface config)\n");
    printf("     ms timeout: Time to wait for a response in milliseconds "
              "(default: 1000). The option affects only timeout in absence "
              "of any responses, otherwise wait for two RTTs\n");
}

static int _configure(int argc, char **argv, _ping_data_t *data)
{
    char *cmdname = argv[0];
    int res = 1;
    int value;

    /* parse command line arguments */
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {

            data->hostname = arg;

            res = netutils_get_ipv6(&data->host, (netif_t **)&data->netif, arg);
            if (res) {
                break;
            }
        }
        else {
            switch (arg[1]) {
                case 'c':
                    if (((i++) + 1) < argc) {
                        data->count = atoi(argv[i]);
                        if (data->count > 0) {
                            continue;
                        }
                    }
                    /* intentionally falls through */
                case 'h':
                    res = 1;
                    continue;
                    /* intentionally falls through */
                case 'i':
                    if ((++i) < argc) {
                        data->interval = (uint32_t)atoi(argv[i]) * US_PER_MS;
                        continue;
                    }
                    /* intentionally falls through */
                case 's':
                    if ((++i) < argc) {
                        value = atoi(argv[i]);

                        if ((value < 0) || ((unsigned)value > (UINT16_MAX - sizeof(icmpv6_hdr_t)))) {
                            printf("ICMPv6 datagram size should be in range 0-65527.\n");
                            return -1;
                        }
                        data->datalen = value;
                        continue;
                    }
                    /* intentionally falls through */
                case 't':
                    if ((++i) < argc) {
                        data->hoplimit = atoi(argv[i]);
                        continue;
                    }
                    /* intentionally falls through */
                case 'W':
                    if ((++i) < argc) {
                        data->timeout = (uint32_t)atoi(argv[i]) * US_PER_MS;
                        if (data->timeout > 0) {
                            continue;
                        }
                    }
                    /* intentionally falls through */
                default:
                    res = 1;
                    break;
            }
        }
    }
    if (res != 0) {
        _usage(cmdname);
    }
    data->id ^= (ztimer_now(ZTIMER_USEC) & UINT16_MAX);
#ifdef MODULE_LUID
    luid_custom(&data->id, sizeof(data->id), data->id);
#endif
    return res;
}

static void _pinger(_ping_data_t *data)
{
    uint32_t timer;
    int res;

    /* schedule next event (next ping or finish) ASAP */
    if ((data->num_sent + 1) < data->count) {
        /* didn't send all pings yet - schedule next in data->interval */
        data->sched_msg.type = _SEND_NEXT_PING;
        timer = data->interval;
    }
    else {
        /* Wait for the last ping to come back.
         * data->timeout: wait for a response in milliseconds.
         * Affects only timeout in absence of any responses,
         * otherwise ping waits for two max RTTs. */
        data->sched_msg.type = _PING_FINISH;
        timer = data->timeout;
        if (data->num_recv) {
            /* approx. 2*tmax, in seconds (2 RTT) */
            timer = (data->tmax / (512UL * 1024UL)) * US_PER_SEC;
            if (timer == 0) {
                timer = 1U * US_PER_SEC;
            }
        }
    }
    ztimer_set_msg(ZTIMER_USEC, &data->sched_timer, timer, &data->sched_msg,
                   thread_getpid());
    bf_unset(data->cktab, (size_t)data->num_sent % CKTAB_SIZE);

    res = gnrc_icmpv6_echo_send(data->netif, &data->host, data->id,
                                data->num_sent++, data->hoplimit, data->datalen);
    switch (-res) {
    case 0:
        break;
    case ENOMEM:
        printf("error: packet buffer full\n");
        break;
    default:
        printf("error: %d\n", res);
        break;
    }
}

static int _print_reply(gnrc_pktsnip_t *pkt, int corrupted, uint32_t triptime, void *ctx)
{
    _ping_data_t *data = ctx;
    gnrc_pktsnip_t *netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    gnrc_pktsnip_t *ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    gnrc_pktsnip_t *icmpv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV6);

    if (!ipv6 || !icmpv6) {
        return -EINVAL;
    }

    ipv6_hdr_t *ipv6_hdr = ipv6->data;
    icmpv6_echo_t *icmpv6_hdr = icmpv6->data;

    kernel_pid_t if_pid = KERNEL_PID_UNDEF;
    int16_t rssi = GNRC_NETIF_HDR_NO_RSSI;
    int16_t truncated;

    if (netif) {
        gnrc_netif_hdr_t *netif_hdr = netif->data;
        if_pid = netif_hdr->if_pid;
        rssi = netif_hdr->rssi;
    }

    /* check if payload size matches expectation */
    truncated = (data->datalen + sizeof(icmpv6_echo_t)) - icmpv6->size;

    if (icmpv6_hdr->type != ICMPV6_ECHO_REP) {
        return -EINVAL;
    }

    char from_str[IPV6_ADDR_MAX_STR_LEN];
    const char *dupmsg = " (DUP!)";
    uint16_t recv_seq;

    /* not our ping */
    if (byteorder_ntohs(icmpv6_hdr->id) != data->id) {
        return -EINVAL;
    }
    if (!ipv6_addr_is_multicast(&data->host) &&
        !ipv6_addr_equal(&ipv6_hdr->src, &data->host)) {
        return -EINVAL;
    }
    recv_seq = byteorder_ntohs(icmpv6_hdr->seq);
    ipv6_addr_to_str(&from_str[0], &ipv6_hdr->src, sizeof(from_str));

    if (gnrc_netif_highlander() || (if_pid == KERNEL_PID_UNDEF) ||
        !ipv6_addr_is_link_local(&ipv6_hdr->src)) {
        printf("%u bytes from %s: icmp_seq=%u ttl=%u",
               (unsigned)icmpv6->size,
               from_str, recv_seq, ipv6_hdr->hl);
    } else {
        printf("%u bytes from %s%%%u: icmp_seq=%u ttl=%u",
               (unsigned)icmpv6->size,
               from_str, if_pid, recv_seq, ipv6_hdr->hl);

    }
    /* check if payload size matches */
    if (truncated) {
        printf(" truncated by %d byte", truncated);
    }
    /* check response for corruption */
    else if (corrupted >= 0) {
        printf(" corrupted at offset %u", (unsigned)corrupted);
    }
    if (rssi != GNRC_NETIF_HDR_NO_RSSI) {
        printf(" rssi=%"PRId16" dBm", rssi);
    }
    /* we can only calculate RTT (triptime) if payload was large enough for
       a TX timestamp */
    if (triptime) {
        printf(" time=%lu.%03lu ms", (long unsigned)triptime / 1000,
               (long unsigned)triptime % 1000);

        data->tsum += triptime;
        if (triptime < data->tmin) {
            data->tmin = triptime;
        }
        if (triptime > data->tmax) {
            data->tmax = triptime;
        }
    }
    if (bf_isset(data->cktab, recv_seq % CKTAB_SIZE)) {
        data->num_rept++;
    }
    else {
        bf_set(data->cktab, recv_seq % CKTAB_SIZE);
        data->num_recv++;
        dupmsg += 7;
    }

    puts(dupmsg);

    return 0;
}

static int _finish(_ping_data_t *data)
{
    unsigned long tmp, nrecv, ndup;

    tmp = data->num_sent;
    nrecv = data->num_recv;
    ndup = data->num_rept;
    printf("\n--- %s PING statistics ---\n"
           "%lu packets transmitted, "
           "%lu packets received, ",
           data->hostname, tmp, nrecv);
    if (ndup) {
        printf("%lu duplicates, ", ndup);
    }
    if (tmp > 0) {
        tmp = ((tmp - nrecv) * 100) / tmp;
    }
    printf("%lu%% packet loss\n", tmp);
    if (data->tmin != UINT_MAX) {
        unsigned tavg = data->tsum / (nrecv + ndup);
        printf("round-trip min/avg/max = %u.%03u/%u.%03u/%u.%03u ms\n",
               data->tmin / 1000, data->tmin % 1000,
               tavg / 1000, tavg % 1000,
               data->tmax / 1000, data->tmax % 1000);
    }
    /* if condition is true, exit with 1 -- 'failure' */
    return (nrecv == 0);
}

#endif /* MODULE_GNRC_ICMPV6 */

/** @} */
