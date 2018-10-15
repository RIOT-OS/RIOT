/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Shell commands for the rdcli module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "net/rdcli.h"
#include "net/nanocoap.h"
#include "net/sock/util.h"
#include "net/rdcli_config.h"

static int make_sock_ep(sock_udp_ep_t *ep, const char *addr)
{
    ep->port = 0;
    if (sock_udp_str2ep(ep, addr) < 0) {
        return -1;
    }
    ep->family  = AF_INET6;
    ep->netif   = SOCK_ADDR_ANY_NETIF;
    if (ep->port == 0) {
        ep->port = RDCLI_SERVER_PORT;
    }
    return 0;
}

int _rdcli_handler(int argc, char **argv)
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
        if (rdcli_register(&remote, regif) != RDCLI_OK) {
            puts("error: registration failed");
        }
        else {
            puts("registration successful\n");
            rdcli_dump_status();
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "discover") == 0)) {
        if (argc < 3) {
            printf("usage: %s discover <server address>\n", argv[0]);
            return 1;
        }
        char regif[NANOCOAP_URI_MAX];
        sock_udp_ep_t remote;
        if (make_sock_ep(&remote, argv[2]) < 0) {
            printf("error: unable to parse address\n");
            return 1;
        }
        if (rdcli_discover_regif(&remote, regif, sizeof(regif)) == RDCLI_OK) {
            printf("the registration interface is '%s'\n", regif);
        }
        else {
            printf("error: unable to discover registration interface\n");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "update") == 0)) {
        res = rdcli_update();
        if (res == RDCLI_OK) {
            puts("RD update successful");
        }
        else if (res == RDCLI_NORD) {
            puts("error: not associated with any RD");
        }
        else if (res == RDCLI_TIMEOUT) {
            puts("error: unable to reach RD - dropped association");
        }
        else {
            puts("error: RD update failed");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "remove") == 0)) {
        res = rdcli_remove();
        if (res == RDCLI_OK) {
            puts("node successfully removed from RD");
        }
        else if (res == RDCLI_NORD) {
            puts("error: not associated with any RD");
        }
        else if (res == RDCLI_TIMEOUT) {
            puts("error: unable to reach RD - remove association only locally");
        }
        else {
            puts("error: unable to remove node from RD");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "info") == 0)) {
        rdcli_dump_status();
    }
    else {
        printf("usage: %s <register|discover|update|remove|info>\n",
               argv[0]);
        return 1;
    }

    return 0;
}
