/*
 * Copyright (c) 2018 Ken Bannister. All rights reserved.
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
 * @brief       nanocoap test app
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "kernel_types.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int nanotest_client_cmd(int argc, char **argv);
extern int nanotest_server_cmd(int argc, char **argv);
static int _list_all_inet6(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "client", "CoAP client", nanotest_client_cmd },
    { "server", "CoAP server", nanotest_server_cmd },
    { "inet6", "IPv6 addresses", _list_all_inet6 },
    { NULL, NULL, NULL }
};

/* _list_all_inet6() and _print_addr() derived from sc_gnrc_netif.c */

static void _print_addr(ipv6_addr_t *addr, uint8_t flags)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    printf("          inet6 addr: ");
    ipv6_addr_to_str(addr_str, addr, sizeof(addr_str));
    printf("%s  scope: ", addr_str);
    if ((ipv6_addr_is_link_local(addr))) {
        printf("link");
    }
    else {
        printf("global");
    }
    if (flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_ANYCAST) {
        printf(" [anycast]");
    }
    if (flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE) {
        printf("  TNT[%u]",
               flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_TENTATIVE);
    }
    else {
        switch (flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) {
            case GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_DEPRECATED:
                printf("  DPR");
                break;
            case GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID:
                printf("  VAL");
                break;
            default:
                printf("  UNK");
                break;
        }
    }
    printf("\n");
}

static int _list_all_inet6(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    gnrc_netif_t *netif = NULL;
    unsigned addr_qty = 0;

    while ((netif = gnrc_netif_iter(netif))) {
        ipv6_addr_t ipv6_addrs[CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF];

        int res = gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                                  sizeof(ipv6_addrs));
        if (res >= 0) {
            uint8_t ipv6_addrs_flags[CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF];

            memset(ipv6_addrs_flags, 0, sizeof(ipv6_addrs_flags));
            /* assume it to succeed (otherwise array will stay 0) */
            gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR_FLAGS, 0,
                            ipv6_addrs_flags, sizeof(ipv6_addrs_flags));
            /* yes, the res of NETOPT_IPV6_ADDR is meant to be here ;-) */
            for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
                _print_addr(&ipv6_addrs[i], ipv6_addrs_flags[i]);
                addr_qty++;
            }
        }
    }
    if (!addr_qty) {
        printf("no addresses found\n");
    }
    return 0;
}

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("nanocoap test app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
