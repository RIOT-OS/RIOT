/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       6LoWPAN example application shell functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>
#include "msg.h"
#include "thread.h"
#include "sched.h"
#include "sixlowpan/ip.h"
#include "inet_pton.h"

#include "sixlowapp.h"

#define ENABLE_DEBUG            (0)
#include "debug.h"

#define ICMP_DATA               "RIOT"
#define ICMP_TIMEOUT            (100)

#define MAX_PAYLOAD_SIZE        (32)

extern uint8_t ipv6_ext_hdr_len;

static char payload[MAX_PAYLOAD_SIZE];

unsigned sixlowapp_waiting_for_pong;
kernel_pid_t sixlowapp_waiter_pid;

void sixlowapp_send_ping(int argc, char **argv)
{
    ipv6_addr_t dest;
    const char *icmp_data = ICMP_DATA;
    
    if (argc != 2) {
        puts("! Invalid number of parameters");
        printf("  usage: %s destination\n", argv[0]);
        return;
    }

    if (!inet_pton(AF_INET6, argv[1], &dest)) {
        printf("! %s is not a valid IPv6 address\n", argv[1]);
        return;
    }

    sixlowapp_ndp_workaround(&dest);

    /* send an echo request */
    icmpv6_send_echo_request(&dest, 1, 1, (uint8_t *) icmp_data, sizeof(icmp_data));

    sixlowapp_waiting_for_pong = 1;
    sixlowapp_waiter_pid = sched_active_pid;
    uint64_t rtt;
    msg_t m;
    m.type = 0;
    rtt = sixlowapp_wait_for_msg_type(&m, timex_set(0, ICMP_TIMEOUT * 1000), ICMP_ECHO_REPLY_RCVD);
    if (sixlowapp_waiting_for_pong == 0) {
        char ts[TIMEX_MAX_STR_LEN];
        printf("Echo reply from %s received, rtt: %s\n", inet_ntop(AF_INET6, &dest,
                                                               addr_str,
                                                               IPV6_MAX_ADDR_STR_LEN),
                                                        timex_to_str(timex_from_uint64(rtt), ts));
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
        puts("  usage: nc [-l] [destination] [port] [text]");
        return;
    }

    if (strlen(argv[1]) == 2) {
        if (strncmp(argv[1], "-l", 2)) {
            puts("! Invalid parameter");
            puts("  usage: nc [-l] [destination] [port] [text]");
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
        sixlowapp_ndp_workaround(&dest);
        size_t plen;
        if (argc > 3 ) {
            plen = (strlen(argv[3]) > MAX_PAYLOAD_SIZE) ? MAX_PAYLOAD_SIZE : strlen(argv[3]) + 1;
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


