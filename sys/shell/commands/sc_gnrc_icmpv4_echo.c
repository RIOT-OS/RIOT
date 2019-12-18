/*
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  William MARTIN <william.martin@power-lan.com>
 *
 * Based on sc_gnrc_icmpv4_echo
 *
 */

#ifdef MODULE_GNRC_ICMPV4
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bitfield.h"
#include "byteorder.h"
#include "kernel_types.h"
#ifdef MODULE_LUID
#include "luid.h"
#endif
#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/icmpv4.h"
#include "net/gnrc/ipv4/hdr.h"
#include "net/icmp.h"
#include "net/ipv4.h"
#include "timex.h"
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
    ipv4_addr_t host;
    char *hostname;
    unsigned long num_sent, num_recv, num_rept;
    unsigned long long tsum;
    unsigned tmin, tmax;
    unsigned count;
    size_t datalen;
    BITFIELD(cktab, CKTAB_SIZE);
    uint32_t timeout;
    uint32_t interval;
    kernel_pid_t iface;
    uint16_t id;
    uint8_t hoplimit;
    uint8_t pattern;
} _ping_data_t;

static void _usage(char *cmdname);
static int _configure(int argc, char **argv, _ping_data_t *data);
static void _pinger(_ping_data_t *data);
static void _print_reply(_ping_data_t *data, gnrc_pktsnip_t *icmpv4,
                         ipv4_addr_t *from, unsigned hoplimit, int16_t rssi);
static void _handle_reply(_ping_data_t *data, gnrc_pktsnip_t *pkt);
static int _finish(_ping_data_t *data);

int _gnrc_icmpv4_ping(int argc, char **argv)
{
    _ping_data_t data = {
        .netreg = GNRC_NETREG_ENTRY_INIT_PID(ICMPV4_ECHO_REP,
                                                 sched_active_pid),
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
    gnrc_netreg_register(GNRC_NETTYPE_ICMPV4, &data.netreg);
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
                msg_send(&msg, sched_active_pid);
                break;
        }
    } while (data.num_recv < data.count);
finish:
    xtimer_remove(&data.sched_timer);
    res = _finish(&data);
    gnrc_netreg_unregister(GNRC_NETTYPE_ICMPV4, &data.netreg);
    for (unsigned i = 0;
         i < cib_avail((cib_t *)&sched_active_thread->msg_queue);
         i++) {
        msg_t msg;

        /* remove all remaining messages (likely caused by duplicates) */
        if ((msg_try_receive(&msg) > 0) &&
            (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) &&
            (((gnrc_pktsnip_t *)msg.content.ptr)->type == GNRC_NETTYPE_ICMPV4)) {
            gnrc_pktbuf_release(msg.content.ptr);
        }
        else {
            /* requeue other packets */
            msg_send(&msg, sched_active_pid);
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
    puts("     ms timeout: Time to wait for a resonse in milliseconds "
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
#ifdef MODULE_SOCK_DNS
            if (sock_dns_query(data->hostname, &data->host, AF_INET6) == 0) {
                continue;
            }
#endif
            data->iface = ipv4_addr_split_iface(data->hostname);
            if (data->iface < KERNEL_PID_UNDEF) {
#if GNRC_NETIF_NUMOF == 1
                gnrc_netif_t *netif = gnrc_netif_iter(NULL);
                if (netif != NULL) {
                    data->iface = netif->pid;
                }
#endif
            }
            if (ipv4_addr_from_str(&data->host, data->hostname) == NULL) {
                break;
            }
            res = 0;
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
#ifdef MODULE_LUID
    luid_custom(&data->id, sizeof(data->id), DEFAULT_ID);
#endif
    return res;
}

static void _pinger(_ping_data_t *data)
{
    gnrc_pktsnip_t *pkt, *tmp;
    ipv4_hdr_t *ipv4;
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
                   sched_active_pid);
    bf_unset(data->cktab, (size_t)data->num_sent % CKTAB_SIZE);
    pkt = gnrc_icmpv4_echo_build(ICMPV4_ECHO_REQ, data->id,
                                 (uint16_t)data->num_sent++,
                                 NULL, data->datalen);
    if (pkt == NULL) {
        puts("error: packet buffer full");
        return;
    }
    databuf = (uint8_t *)(pkt->data) + sizeof(icmpv4_echo_t);
    memset(databuf, data->pattern, data->datalen);
    tmp = gnrc_ipv4_hdr_build(pkt, NULL, &data->host);
    if (tmp == NULL) {
        puts("error: packet buffer full");
        goto error_exit;
    }
    pkt = tmp;
    ipv4 = pkt->data;
    /* if data->hoplimit is unset (i.e. 0) gnrc_ipv4 will select hop limit */
    ipv4->ttl = data->hoplimit;
    if (data->iface > KERNEL_PID_UNDEF) {
        gnrc_netif_hdr_t *netif;

        tmp = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (tmp == NULL) {
            puts("error: packet buffer full");
            goto error_exit;
        }
        netif = tmp->data;
        netif->if_pid = data->iface;
        LL_PREPEND(pkt, tmp);
    }
    if (data->datalen >= sizeof(uint32_t)) {
        *((uint32_t *)databuf) = xtimer_now_usec();
    }
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV4,
                                   GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        puts("error: unable to send ICMPv4 echo request");
        goto error_exit;
    }
    return;
error_exit:
    gnrc_pktbuf_release(pkt);
}

static void _print_reply(_ping_data_t *data, gnrc_pktsnip_t *icmpv4,
                         ipv4_addr_t *from, unsigned hoplimit,
                         int16_t rssi)
{
    icmpv4_echo_t *icmpv4_hdr = icmpv4->data;

    /* discard if too short */
    if (icmpv4->size < (data->datalen + sizeof(icmpv4_echo_t))) {
        return;
    }
    if (icmpv4_hdr->type == ICMPV4_ECHO_REP) {
        char from_str[IPV4_ADDR_MAX_STR_LEN];
        const char *dupmsg = " (DUP!)";
        uint32_t triptime = 0;
        uint16_t recv_seq;

        /* not our ping */
        if (byteorder_ntohs(icmpv4_hdr->id) != data->id) {
            return;
        }
        recv_seq = byteorder_ntohs(icmpv4_hdr->sn);
        ipv4_addr_to_str(&from_str[0], from, sizeof(from_str));
        if (data->datalen >= sizeof(uint32_t)) {
            triptime = xtimer_now_usec() - *((uint32_t *)(icmpv4_hdr + 1));
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
        printf("%u bytes from %s: icmp_seq=%u ttl=%u", (unsigned)icmpv4->size,
               from_str, recv_seq, hoplimit);
        if (rssi) {
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
    gnrc_pktsnip_t *netif, *ipv4, *icmpv4;
    gnrc_netif_hdr_t *netif_hdr;
    ipv4_hdr_t *ipv4_hdr;

    netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    ipv4 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV4);
    icmpv4 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_ICMPV4);
    if ((ipv4 == NULL) || (icmpv4 == NULL)) {
        puts("No IPv4 or ICMPv4 header found in reply");
        return;
    }
    ipv4_hdr = ipv4->data;
    netif_hdr = netif ? netif->data : NULL;
    _print_reply(data, icmpv4, &ipv4_hdr->src, ipv4_hdr->ttl, netif_hdr ? netif_hdr->rssi : 0);
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

#endif /* MODULE_GNRC_ICMPV4 */

/** @} */
