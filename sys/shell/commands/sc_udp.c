/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for using UDP
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdio.h>
#include <string.h>

#include "net/ng_netbase.h"
#include "net/ng_udp.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"

#ifdef MODULE_NG_PKTDUMP
static ng_netreg_entry_t _server = {NULL,
                                    NG_NETREG_DEMUX_CTX_ALL,
                                    KERNEL_PID_UNDEF};
#endif


void _send(char *addr, char *port, char *data)
{
    uint16_t port;
    ng_pktsnip_t *payload, *udp, *ip;
    ipv6_addr_t ip_addr;
    ng_netreg_entry_t *sendto;

    /* parse destination address
     *
     * For the future: we should read the type of address from the addr string,
     * so figure out if IPv4 or IPv6 and build the header accordingly...
     * So only IPv6 support for now */
    if (ng_ipv6_addr_from_str(&ip_addr, addr) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    /* parse port */
    port = (uint16_t)port;
    if (port == 0) {
        puts("Error: unable to parse destination port");
        return;
    }

    /* allocate payload */
    payload = ng_pktbuf_add(NULL, data, strlen(data), NG_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return;
    }
    /* allocate UDP header */
    udp = ng_udp_hdr_build(payload, NULL, 0, (uint8_t *)&port, 2);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        ng_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header */
    ip = ng_ipv6_hdr_build(udp, NULL, 0, (uint8_t *)&ip_addr, sizeof(ip_addr));
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
    ng_pktbuf_hold(ip, ng_netreg_num(NG_NETTYPE_UDP, NG_NETREG_DEMUX_CTX_ALL) - 1);
    while (sendto != NULL) {
        ng_netapi_send(sendto->pid, ip);
        sendto = ng_netreg_getnext(sendto);
    }
    printf("Success: send %i byte to %s:%s\n", payload->size, addr, port);
}

#ifdef MODULE_NG_PKTDUMP
void _start_server(char *port_str)
{
    uint16_t port;

    /* check if server is already running */
    if (_server->pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu32 "\n",
                _server->demux_ctx);
        return;
    }
    /* parse port */
    port = (uint16_t)atoi(port_str)
    if (port == 0) {
        puts("Error: invalid port specified");
        return;
    }
    /* start server (which means registering pktdump for the chosen port) */
    _server->pid = ng_pktdump_getpid();
    _server->demux_ctx = (uint32_t)port;
    ng_netreg_register(NG_NETTYPE_UDP, &_server);
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
}

void _stop_server(void)
{
    /* check if server is running at all */
    if (_server->pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return;
    }
    /* stop server */
    ng_netreg_unregister(NG_NETTYPE_UDP, &_server);
    _server->pid = KERNEL_PID_UNDEF;
    puts("Success: stopped UDP server");
}
#endif

void _udp(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
    }

    if (strcmp(argv[1], "send") == 0) {
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <data>\n", argv[0]);
            return;
        }
        _send(argv[2], argv[3], argv[4]);
    }
#ifdef MODULE_NG_PKTDUMP
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return;
            }
            _start_server(argv[3]);
        }
        else if (strcmp(argv[2], "stop") == 0) {
            _stop_server();
        }
        else {
            puts("error: invalid command");
        }
    }
#endif
    else {
        puts("error: invalid command");
    }
}
