/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       6LoWPAN example application helper functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"
#include "thread.h"
#include "sched.h"
#include "sixlowpan/ip.h"
#include "transceiver.h"
#include "ieee802154_frame.h"
#include "inet_pton.h"

#include "sixlowapp.h"

#define ENABLE_DEBUG            (0)
#include "debug.h"

#define RCV_BUFFER_SIZE         (32)

#define ICMP_DATA               "RIOT"
#define ICMP_TIMEOUT            (100)

#define IPV6_HDR_LEN            (0x28)
#define MAX_PAYLOAD_SIZE        (32)

#define ICMP_ECHO_REPLY_RCVD    (4444)

extern uint8_t ipv6_ext_hdr_len;

msg_t msg_q[RCV_BUFFER_SIZE];
static char payload[MAX_PAYLOAD_SIZE];

static unsigned _waiting_for_pong;
static kernel_pid_t _waiter_pid;

/* --------------- internal functions ------------------*/
static void _ndp_workaround(ipv6_addr_t *dest)
{
    /* add the destination to the neighbor cache if is not already in it */
    if (!ndp_neighbor_cache_search(dest)) {
        DEBUGF("XXX: Adding %s to neighbor cache.\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest));
        ndp_neighbor_cache_add(IF_ID, dest, &(dest->uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE, 0xffff);
    }
}

static uint64_t _wait_for_msg_type(msg_t *m, timex_t timeout, uint16_t mtype)
{
    timex_t t1, t2, delta;
    delta = timex_set(0, 0);
    vtimer_now(&t1);
    while (timex_cmp(delta, timeout) < 0) {
        if (vtimer_msg_receive_timeout(m, timeout) < 0) {
            return 0;
        }
        vtimer_now(&t2);
        delta = timex_sub(t2, t1);
        if (m->type == mtype) {
            return timex_uint64(delta);
        }
        timeout = timex_sub(timeout, delta);
    }
    return 0;
}

/* --------------- public functions --------------------*/
void sixlowapp_send_ping(int argc, char **argv)
{
    ipv6_addr_t dest;
    const char *icmp_data = ICMP_DATA;
    
    if (argc != 2) {
        puts("! Not enough parameters");
        printf("  usage: %s destination\n", argv[0]);
        return;
    }

    if (!inet_pton(AF_INET6, argv[1], &dest)) {
        printf("! %s is not a valid IPv6 address\n", argv[1]);
        return;
    }

    _ndp_workaround(&dest);

    /* send an echo request */
    icmpv6_send_echo_request(&dest, 1, 1, (uint8_t *) icmp_data, sizeof(icmp_data));

    _waiting_for_pong = 1;
    _waiter_pid = sched_active_pid;
    uint64_t rtt;
    msg_t m;
    m.type = 0;
    rtt = _wait_for_msg_type(&m, timex_set(0, ICMP_TIMEOUT * 1000), ICMP_ECHO_REPLY_RCVD);
    if (_waiting_for_pong == 0) {
        printf("Echo reply from %s received, rtt: ", inet_ntop(AF_INET6, &dest,
                                                               addr_str,
                                                               IPV6_MAX_ADDR_STR_LEN));
        timex_print(timex_from_uint64(rtt));
    }
    else {
        printf("! Destination %s is unreachable\n", inet_ntop(AF_INET6,
                                                              &dest,
                                                              addr_str,
                                                              IPV6_MAX_ADDR_STR_LEN));
    }
}

void sixlowapp_netcat(int argc, char **argv)
{
    ipv6_addr_t dest;
    
    if (argc < 3) {
        puts("! Not enough parameters");
        puts("  usage: nc [-l] [destination] [port]");
        return;
    }

    if (strlen(argv[1]) == 2) {
        if (strncmp(argv[1], "-l", 2)) {
            puts("! Invalid parameter");
            puts("  usage: nc [-l] [destination] [port]");
            return;
        }
        else {
            sixlowapp_netcat_listen_port = atoi(argv[2]);
            thread_wakeup(sixlowapp_udp_server_pid);
        }
    }
    else if (!inet_pton(AF_INET6, argv[1], &dest)) {
        printf("! %s is not a valid IPv6 address\n", argv[1]);
    }
    else {
        _ndp_workaround(&dest);
        size_t plen;
        if (argc > 3 ) {
            plen = (strlen(argv[3]) > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : strlen(argv[1]) + 1;
            memcpy(payload, argv[3], plen);
            payload[plen - 1] = 0;
        }
        else {
            plen = 5;
            strncpy(payload, "RIOT", plen);

        }
        sixlowapp_udp_send(&dest, atoi(argv[2]), payload, plen);
    }
}

void *sixlowapp_monitor(void *unused)
{
    (void) unused;

    msg_t m;
    ipv6_hdr_t *ipv6_buf;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == IPV6_PACKET_RECEIVED) {
            ipv6_buf = (ipv6_hdr_t *) m.content.ptr;

            if (ipv6_buf->nextheader == IPV6_PROTO_NUM_ICMPV6) {
                icmpv6_hdr_t *icmpv6_buf = (icmpv6_hdr_t *) &((uint8_t*)ipv6_buf)[(IPV6_HDR_LEN)];
                if (icmpv6_buf->type == ICMPV6_TYPE_ECHO_REPLY) {
                    if (_waiting_for_pong) {
                        msg_t m;
                        m.type = ICMP_ECHO_REPLY_RCVD;
                        _waiting_for_pong = 0;
                        msg_send(&m, _waiter_pid, false);
                    }
                }
            }
            /* add the destination to the neighbor cache if is not already in it */
            if (!ndp_neighbor_cache_search(&(ipv6_buf->srcaddr))) {
                    ndp_neighbor_cache_add(IF_ID, &(ipv6_buf->srcaddr),
                                            &(ipv6_buf->srcaddr.uint16[7]), 2,
                                            0, NDP_NCE_STATUS_REACHABLE,
                                            NDP_NCE_TYPE_TENTATIVE, 0xffff);
            }
            DEBUGF("IPv6 datagram received (next header: %02X)", ipv6_buf->nextheader);
            DEBUG(" from %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                                 &ipv6_buf->srcaddr));
        }
        else {
            printf("! Unknown message received, type %04X\n", m.type);
        }
    }
}
