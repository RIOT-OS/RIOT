/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
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
 * @brief       Shell commands for the cord_ep module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "net/cord/config.h"
#include "net/cord/ep.h"
#include "net/gnrc/netif.h"
#include "net/nanocoap.h"
#include "net/sock/util.h"
#include "uri_parser.h"
#include "shell.h"


static int _make_sock_ep(sock_udp_ep_t *ep, const char *addr)
{
    ep->port = 0;
    if (sock_udp_name2ep(ep, addr) < 0) {
        return -1;
    }
    /* if netif not specified in addr */
    if ((ep->netif == SOCK_ADDR_ANY_NETIF) && (gnrc_netif_numof() == 1)) {
        /* assign the single interface found in gnrc_netif_numof() */
        ep->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
    }
    ep->family = AF_INET6;
    if (ep->port == 0) {
        ep->port = COAP_PORT;
    }
    return 0;
}

static int _cord_ep_handler(int argc, char **argv)
{
    int res;

    if ((argc > 1) && (strcmp(argv[1], "register") == 0)) {
        if (argc < 3) {
            printf("usage: %s register <registration uri>\n", argv[0]);
            puts(
                "If the registration URI's path is empty, the registration resource is auto-discovered");
            printf("example: %s register coap://[2001:db8::1]:99\n", argv[0]);
            return 1;
        }

        uri_parser_result_t uri_result;
        if (uri_parser_process_string(&uri_result, argv[2]) != 0) {
            puts("error: unable to parse uri");
            return 1;
        }
        sock_udp_ep_t remote;
        remote.family = AF_INET6;
        remote.netif = SOCK_ADDR_ANY_NETIF;
        remote.port = uri_result.port;

        if ((uri_result.scheme_len == 4) && (strncmp(uri_result.scheme, "coap", 4) == 0)) {
            if (uri_result.port == 0) {
                remote.port = COAP_PORT;
            }
        }
        else {
            puts("error: Only coap schemes are supported");
            return 1;
        }

        if (uri_result.ipv6addr == NULL) {
            puts("error: Only ipv6 addresses are supported");
            return 1;
        }

        if (!ipv6_addr_from_buf((ipv6_addr_t *)&remote.addr.ipv6, uri_result.ipv6addr,
                                uri_result.ipv6addr_len)) {
            puts("error: IPv6 address malformed");
            return 1;
        }

        puts("Registering with RD now, this may take a short while...");
        if (cord_ep_register(&remote, uri_result.path) != CORD_EP_OK) {
            puts("failure: registration failed");
        }
        else {
            puts("registration successful\n");
            cord_ep_dump_status();
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "discover") == 0)) {
        if (argc < 3) {
            printf("usage: %s discover <server address>\n", argv[0]);
            return 1;
        }
        char regif[CONFIG_NANOCOAP_URI_MAX];
        sock_udp_ep_t remote;
        if (_make_sock_ep(&remote, argv[2]) < 0) {
            printf("error: unable to parse address\n");
            return 1;
        }
        if (cord_ep_discover_regif(&remote, regif, sizeof(regif)) == CORD_EP_OK) {
            printf("the registration interface is '%s'\n", regif);
        }
        else {
            printf("error: unable to discover registration interface\n");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "update") == 0)) {
        res = cord_ep_update();
        if (res == CORD_EP_OK) {
            puts("RD update successful");
        }
        else if (res == CORD_EP_NORD) {
            puts("error: not associated with any RD");
        }
        else if (res == CORD_EP_TIMEOUT) {
            puts("error: unable to reach RD - dropped association");
        }
        else {
            puts("error: RD update failed");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "remove") == 0)) {
        res = cord_ep_remove();
        if (res == CORD_EP_OK) {
            puts("node successfully removed from RD");
        }
        else if (res == CORD_EP_NORD) {
            puts("error: not associated with any RD");
        }
        else if (res == CORD_EP_TIMEOUT) {
            puts("error: unable to reach RD - remove association only locally");
        }
        else {
            puts("error: unable to remove node from RD");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "info") == 0)) {
        cord_ep_dump_status();
    }
    else {
        printf("usage: %s <register|discover|update|remove|info>\n",
               argv[0]);
        return 1;
    }

    return 0;
}

SHELL_COMMAND(cord_ep, "Resource directory endpoint commands", _cord_ep_handler);
