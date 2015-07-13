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
 * @brief       Demonstrating the sending and receiving of UDP data
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "kernel.h"
#include "net/ng_netbase.h"
#include "net/ng_ipv6.h"
#include "net/ng_udp.h"
#include "net/ng_pktdump.h"

static ng_netreg_entry_t server = {NULL, NG_NETREG_DEMUX_CTX_ALL,
                                   KERNEL_PID_UNDEF};


static void send(char *addr_str, char *port_str, char *data)
{
    uint8_t port[2];
    uint16_t tmp;
    ng_pktsnip_t *payload, *udp, *ip;
    ng_ipv6_addr_t addr;
    ng_netreg_entry_t *sendto;

    /* parse destination address */
    if (ng_ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    /* parse port */
    tmp = (uint16_t)atoi(port_str);
    if (tmp == 0) {
        puts("Error: unable to parse destination port");
        return;
    }
    port[0] = (uint8_t)tmp;
    port[1] = tmp >> 8;

    /* allocate payload */
    payload = ng_pktbuf_add(NULL, data, strlen(data), NG_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return;
    }
    /* allocate UDP header, set source port := destination port */
    udp = ng_udp_hdr_build(payload, port, 2, port, 2);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        ng_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header */
    ip = ng_ipv6_hdr_build(udp, NULL, 0, (uint8_t *)&addr, sizeof(addr));
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        ng_pktbuf_release(udp);
        return;
    }
    /* send packet */
    sendto = ng_netreg_lookup(NG_NETTYPE_UDP, NG_NETREG_DEMUX_CTX_ALL);
    if (sendto == NULL) {
        puts("Error: unable to locate UDP thread");
        ng_pktbuf_release(ip);
        return;
    }
    ng_pktbuf_hold(ip, ng_netreg_num(NG_NETTYPE_UDP,
                                     NG_NETREG_DEMUX_CTX_ALL) - 1);
    while (sendto != NULL) {
        ng_netapi_send(sendto->pid, ip);
        sendto = ng_netreg_getnext(sendto);
    }
    printf("Success: send %i byte to %s:%u\n", payload->size, addr_str, tmp);
}

static void start_server(char *port_str)
{
    uint16_t port;

    /* check if server is already running */
    if (server.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu32 "\n",
                server.demux_ctx);
        return;
    }
    /* parse port */
    port = (uint16_t)atoi(port_str);
    if (port == 0) {
        puts("Error: invalid port specified");
        return;
    }
    /* start server (which means registering pktdump for the chosen port) */
    server.pid = ng_pktdump_getpid();
    server.demux_ctx = (uint32_t)port;
    ng_netreg_register(NG_NETTYPE_UDP, &server);
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
}

static void stop_server(void)
{
    /* check if server is running at all */
    if (server.pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return;
    }
    /* stop server */
    ng_netreg_unregister(NG_NETTYPE_UDP, &server);
    server.pid = KERNEL_PID_UNDEF;
    puts("Success: stopped UDP server");
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <data>\n", argv[0]);
            return 1;
        }
        send(argv[2], argv[3], argv[4]);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            start_server(argv[3]);
        }
        else if (strcmp(argv[2], "stop") == 0) {
            stop_server();
        }
        else {
            puts("error: invalid command");
        }
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}
