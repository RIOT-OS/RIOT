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

#include "net/cord/ep.h"
#include "net/nanocoap.h"
#include "net/sock/util.h"
#include "net/cord/config.h"

static int make_sock_ep(sock_udp_ep_t *ep, const char *addr)
{
    ep->port = 0;
    if (sock_udp_str2ep(ep, addr) < 0) {
        return -1;
    }
    /* if netif not specified in addr */
    if ((ep->netif == SOCK_ADDR_ANY_NETIF) && (gnrc_netif_numof() == 1)) {
        /* assign the single interface found in gnrc_netif_numof() */
        ep->netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
    }
    ep->family  = AF_INET6;
    if (ep->port == 0) {
        ep->port = COAP_PORT;
    }
    return 0;
}

int _cord_ep_handler(int argc, char **argv)
{
    int res;

    if ((argc > 1) && (strcmp(argv[1], "register") == 0)) {
        char *regif = NULL;
        if (argc < 3) {
            printf("usage: %s register <server address> [registration interface]\n",
                   argv[0]);
            return 1;
        }
        sock_udp_ep_t remote;
        if (make_sock_ep(&remote, argv[2]) < 0) {
            printf("error: unable to parse address\n");
            return 1;
        }
        if (argc > 3) {
            regif = argv[3];
        }
        puts("Registering with RD now, this may take a short while...");
        if (cord_ep_register(&remote, regif) != CORD_EP_OK) {
            puts("error: registration failed");
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
        if (make_sock_ep(&remote, argv[2]) < 0) {
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
