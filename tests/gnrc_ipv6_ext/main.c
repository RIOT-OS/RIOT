/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void _send_packet(void) {
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    gnrc_netif_get(ifs);

    addr.u8[0] = 0xfd;
    addr.u8[1] = 0x01;
    addr.u8[15] = 0x02;
    /* fd01::02 */
    gnrc_ipv6_netif_add_addr(ifs[0], &addr, 64, GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST);

    addr.u8[15] = 0x03;
    /* fd01::03 */
    gnrc_ipv6_netif_add_addr(ifs[0], &addr, 64, GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST);

    uint8_t data[] = {
        /* IPv6 Header */
        0x60, 0x00, 0x00, 0x00, /* version, traffic class, flow label */
        0x00, 0x28, /* payload length: 40 */
        0x00, /* next header: Hop-by-Hop Option */
        0x10, /* hop limit: 16 */
        /* source address: fd01::1 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01,
        /* destination address: fd01::2 */
        0xfd, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02,

        /* Hop-by-Hop Options Header */
        /* https://tools.ietf.org/html/rfc6553 */
        0x2b, /* next header: IPv6-Route */
        0x00, /* hdr ext len: 0 * 8 + 8 = 8 */
        0x63, /* option type: RPL Option */
        0x04, /* opt data len: 4 */
        0x80, /* flags, Down: 1, Rank-Error: 0, Forwarding-Error: 0 */
        0x00, /* RPLInstanceID */
        0x80, 0x00, /* SenderRank */

        /* RPL Routing Header */
        /* https://tools.ietf.org/html/rfc6554 */
        0x11, /* next header: UDP */
        0x02, /* hdr ext len: 2 * 8 + 8 = 24 */
        0x03, /* routing type: SRH */
        0x02, /* segments left: 2 */
        0xef, /* ComprI: 14, ComprE: 15 */
        0xd0, 0x00, 0x00, /* pad and reserved */
        /* address: fd01::3, fd01::2 */
        0x00, 0x03, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,

        /* UDP (ignored) */
        0x1f, 0x90, /* source port: 8080 */
        0x1f, 0x90, /* destination port: 8080 */
        0x00, 0x08, /* length: 8 */
        0xff, 0xff, /* checksum */
    };

    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, data, sizeof(data), GNRC_NETTYPE_UNDEF);

    gnrc_netapi_dispatch_receive(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt);

    printf("pkt->users: %d\n", pkt->users);
    assert(pkt->users == 0);
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    _send_packet();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
