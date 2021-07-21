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
#include "sched.h"
#ifdef MODULE_LUID
#include "luid.h"
#endif
#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/icmpv6.h"
#ifdef MODULE_GNRC_IPV6_NIB
#include "net/gnrc/ipv6/nib/nc.h"
#endif
#include "net/icmpv6.h"
#include "net/ipv6.h"
#include "net/utils.h"
#include "timex.h"
#include "unaligned.h"
#include "utlist.h"
#include "xtimer.h"

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
    xtimer_t sched_timer;
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
    uint8_t pattern;
} _ping_data_t;

static void _usage(char *cmdname);
static int _configure(int argc, char **argv, _ping_data_t *data);
static void _pinger(_ping_data_t *data);
static void _print_reply(_ping_data_t *data, gnrc_pktsnip_t *icmpv6,
                         ipv6_addr_t *from, unsigned hoplimit, gnrc_netif_hdr_t *netif_hdr);
static void _handle_reply(_ping_data_t *data, gnrc_pktsnip_t *pkt);
static int _finish(_ping_data_t *data);

int _gnrc_icmpv6_ping(int argc, char **argv)
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
        .pattern = DEFAULT_ID,
    };
    int res;

    if ((res = _configure(argc, argv, &data)) != 0) {
        return res;
    }
    gnrc_netreg_register(GNRC_NETTYPE_ICMPV6, &data.netreg);
    _pinger(&data);
    do {
        msg_t msg;

        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV: {
                _handle_reply(&data, msg.content.ptr);
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
    xtimer_remove(&data.sched_timer);
    res = _finish(&data);
    gnrc_netreg_unregister(GNRC_NETTYPE_ICMPV6, &data.netreg);
    for (unsigned i = 0;
         i < (unsigned)msg_avail();
         i++) {
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
    return res;
}

static void _usage(char *cmdname)
{
    printf("%s [-c <count>] [-h] [-i <ms interval>] [-s <packetsize>]\n",
           cmdname);
    puts("     [-t hoplimit] [-W <ms timeout>] <host>[%<interface>]");
    puts("     count: number of pings (default: 3)");
    puts("     ms interval: wait interval milliseconds between sending "
              "(default: 1000)");
    puts("     packetsize: number of bytes in echo payload; must be >= 4 to "
              "measure round trip time (default: 4)");
    puts("     hoplimit: Set the IP time to life/hoplimit "
              "(default: interface config)");
    puts("     ms timeout: Time to wait for a response in milliseconds "
              "(default: 1000). The option affects only timeout in absence "
              "of any responses, otherwise wait for two RTTs");
}

static int _configure(int argc, char **argv, _ping_data_t *data)
{
    char *cmdname = argv[0];
    int res = 1;

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
                        data->datalen = atoi(argv[i]);
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
    data->id ^= (xtimer_now_usec() & UINT16_MAX);
#ifdef MODULE_LUID
    luid_custom(&data->id, sizeof(data->id), data->id);
#endif
    return res;
}

static void _pinger(_ping_data_t *data)
{
    gnrc_pktsnip_t *pkt, *tmp;
    ipv6_hdr_t *ipv6;
    uint32_t timer;
    uint8_t *databuf;

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
    xtimer_set_msg(&data->sched_timer, timer, &data->sched_msg,
                   thread_getpid());
    bf_unset(data->cktab, (size_t)data->num_sent % CKTAB_SIZE);
    pkt = gnrc_icmpv6_echo_build(ICMPV6_ECHO_REQ, data->id,
                                 (uint16_t)data->num_sent++,
                                 NULL, data->datalen);
    if (pkt == NULL) {
        puts("error: packet buffer full");
        return;
    }
    databuf = (uint8_t *)(pkt->data) + sizeof(icmpv6_echo_t);
    memset(databuf, data->pattern, data->datalen);
    tmp = gnrc_ipv6_hdr_build(pkt, NULL, &data->host);
    if (tmp == NULL) {
        puts("error: packet buffer full");
        goto error_exit;
    }
    pkt = tmp;
    ipv6 = pkt->data;
    /* if data->hoplimit is unset (i.e. 0) gnrc_ipv6 will select hop limit */
    ipv6->hl = data->hoplimit;
    if (data->netif != NULL) {
        tmp = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (tmp == NULL) {
            puts("error: packet buffer full");
            goto error_exit;
        }
        gnrc_netif_hdr_set_netif(tmp->data, data->netif);
        pkt = gnrc_pkt_prepend(pkt, tmp);
    }
    if (data->datalen >= sizeof(uint32_t)) {
        uint32_t now = xtimer_now_usec();
        memcpy(databuf, &now, sizeof(now));
    }
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                   GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        puts("error: unable to send ICMPv6 echo request");
        goto error_exit;
    }
    return;
error_exit:
    gnrc_pktbuf_release(pkt);
}

static void _print_reply(_ping_data_t *data, gnrc_pktsnip_t *icmpv6,
                         ipv6_addr_t *from, unsigned hoplimit,
                         gnrc_netif_hdr_t *netif_hdr)
{
    icmpv6_echo_t *icmpv6_hdr = icmpv6->data;

    kernel_pid_t if_pid = netif_hdr ? netif_hdr->if_pid : KERNEL_PID_UNDEF;
    int16_t rssi = netif_hdr ? netif_hdr->rssi : GNRC_NETIF_HDR_NO_RSSI;

    /* discard if too short */
    if (icmpv6->size < (data->datalen + sizeof(icmpv6_echo_t))) {
        return;
    }
    if (icmpv6_hdr->type == ICMPV6_ECHO_REP) {
        char from_str[IPV6_ADDR_MAX_STR_LEN];
        const char *dupmsg = " (DUP!)";
        uint32_t triptime = 0;
        uint16_t recv_seq;

        /* not our ping */
        if (byteorder_ntohs(icmpv6_hdr->id) != data->id) {
            return;
        }
        if (!ipv6_addr_is_multicast(&data->host) &&
            !ipv6_addr_equal(from, &data->host)) {
            return;
        }
        recv_seq = byteorder_ntohs(icmpv6_hdr->seq);
        ipv6_addr_to_str(&from_str[0], from, sizeof(from_str));
        if (data->datalen >= sizeof(uint32_t)) {
            triptime = xtimer_now_usec() - unaligned_get_u32(icmpv6_hdr + 1);
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
        if (gnrc_netif_highlander() || (if_pid == KERNEL_PID_UNDEF) ||
            !ipv6_addr_is_link_local(from)) {
            printf("%u bytes from %s: icmp_seq=%u ttl=%u",
                   (unsigned)icmpv6->size,
                   from_str, recv_seq, hoplimit);
        } else {
            printf("%u bytes from %s%%%u: icmp_seq=%u ttl=%u",
                   (unsigned)icmpv6->size,
                   from_str, if_pid, recv_seq, hoplimit);

        }
        if (rssi != GNRC_NETIF_HDR_NO_RSSI) {
            printf(" rssi=%"PRId16" dBm", rssi);
        }
        if (data->datalen >= sizeof(uint32_t)) {
            printf(" time=%lu.%03lu ms", (long unsigned)triptime / 1000,
                   (long unsigned)triptime % 1000);
        }
        puts(dupmsg);
    }
}

static void _handle_reply(_ping_data_t *data, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *netif, *ipv6, *icmpv6;
    gnrc_netif_hdr_t *netif_hdr;
    ipv6_hdr_t *ipv6_hdr;

    netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    icmpv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV6);
    if ((ipv6 == NULL) || (icmpv6 == NULL)) {
        puts("No IPv6 or ICMPv6 header found in reply");
        return;
    }
    ipv6_hdr = ipv6->data;
    netif_hdr = netif ? netif->data : NULL;
    _print_reply(data, icmpv6, &ipv6_hdr->src, ipv6_hdr->hl, netif_hdr);
#ifdef MODULE_GNRC_IPV6_NIB
    /* successful ping to neighbor (NIB handles case if ipv6->src is not a
     * neighbor) can be taken as upper-layer hint for reachability:
     * https://tools.ietf.org/html/rfc4861#section-7.3.1 */
    gnrc_ipv6_nib_nc_mark_reachable(&ipv6_hdr->src);
#endif
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
