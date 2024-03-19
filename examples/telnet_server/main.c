/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief       Example application for demonstrating the RIOT telnet server
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "net/ipv6/addr.h"
#include "net/netif.h"
#include "net/telnet.h"
#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void _print_motd(void)
{
    puts("RIOT telnet example application");

    puts("╔═══════════════════════════════════════════════════╗");
    puts("║telnet is entirely unencrypted and unauthenticated.║");
    puts("║Do not use this on public networks.                ║");
    puts("╚═══════════════════════════════════════════════════╝");
}

void telnet_cb_pre_connected(sock_tcp_t *sock)
{
    sock_tcp_ep_t ep;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    sock_tcp_get_local(sock, &ep);
    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)ep.addr.ipv6, sizeof(addr_str));

    printf("%s connected\n", addr_str);
}

/* shell lock module makes use of disconnect callback */
#ifndef MODULE_SHELL_LOCK
void telnet_cb_disconneced(void)
{
    puts("disconnected");
}
#endif

void telnet_cb_connected(sock_tcp_t *sock)
{
    (void)sock;
    _print_motd();
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    _print_motd();

    /* print address(es) so we can connect to it */
    printf("{\"IPv6 addresses\": [\"");
    netifs_print_ipv6("\", \"");
    puts("\"]}");

    /* start shell */
    printf("All up, awaiting connection on port %u\n", CONFIG_TELNET_PORT);
    puts("Local shell disabled");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
