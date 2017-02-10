/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demonstrating the sending and receiving of UDP data over POSIX sockets.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdatomic.h>
#include <stdio.h>

#include "byteorder.h"
#include "net/icmpv6.h"
#include "net/ipv6.h"
#include "xtimer.h"

#include "uip.h"
#include "uip-icmp6.h"

#include "common.h"

#define ECHO_ID     (0xd1e9)

static struct uip_icmp6_echo_reply_notification recv_ntfy = { NULL, NULL };
static uint16_t seq = 0;
static atomic_int received = ATOMIC_VAR_INIT(0);
static atomic_int num = ATOMIC_VAR_INIT(0);

static bool _waiting = true;

static inline icmpv6_echo_t *uip_icmp_buf(void)
{
    return ((icmpv6_echo_t *)&uip_buf[uip_l2_l3_hdr_len]);
}

static inline int max_len(void)
{
    return UIP_BUFSIZE - (((uint8_t *)uip_icmp_buf()) - uip_buf) -
           sizeof(icmpv6_echo_t);
}

static int ping_send(const uip_ipaddr_t *dst, int payload_len)
{
    int len = payload_len;
    icmpv6_echo_t *ping = uip_icmp_buf();

    ping->id = byteorder_htons(ECHO_ID);

    if (payload_len > max_len()) {
        puts("Payload too long for buffer.");
        return -1;
    }

    for (network_uint16_t *payload = (network_uint16_t *)(ping + 1);
         len >= 0;
         payload++, len -= 2) {
        *payload = byteorder_htons(seq);
    }

    ping->seq = byteorder_htons(seq++);

    uip_icmp6_send((const uip_ipaddr_t *)dst, ICMPV6_ECHO_REQ, 0,
                   payload_len + (sizeof(icmpv6_echo_t) - sizeof(icmpv6_hdr_t)));

    return 0;
}

static void handle_reply(uip_ipaddr_t *source, uint8_t ttl, uint8_t *data,
                         uint16_t datalen)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    icmpv6_echo_t *ping = (icmpv6_echo_t *)data;

    _waiting = false;

    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)source, sizeof(addr_str));

    atomic_fetch_add(&received, 1);
    printf("%" PRIu16 " bytes from %s: icmp_seq=%" PRIu16 " ttl=%u quota=%i/%i\n",
           datalen, addr_str, byteorder_ntohs(ping->seq), (unsigned)ttl,
           atomic_load(&received), atomic_load(&num));
}

void usage(char *cmd)
{
    printf("usage: %s <dst> [<num>] [<payload_len>]\n", cmd);
}

int ping_cmd(int argc, char **argv)
{
    ipv6_addr_t dst;
    int payload_len, _num;

    if ((argc < 2) || (ipv6_addr_from_str(&dst, argv[1]) == NULL)) {
        usage(argv[0]);
        return 1;
    }
    if ((argc < 3) || ((_num = atoi(argv[2])) == 0)) {
        _num = 3;
    }
    if ((argc < 4) || ((payload_len = atoi(argv[3])) == 0)) {
        payload_len = 16;
    }
    atomic_store(&num, _num);
    atomic_store(&received, 0);
    seq = 0;
    if (recv_ntfy.callback == NULL) {
        uip_icmp6_echo_reply_callback_add(&recv_ntfy, handle_reply);
    }
    for (uint16_t i = 0; i < _num; i++) {
        _waiting = true;
        ping_send((uip_ipaddr_t *)&dst, payload_len);
        xtimer_usleep(1000000);
        if (_waiting) {
            puts("Timeout");
        }
    }

    return 0;
}

/** @} */
