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

#include "net/nanocoap_sock.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if IS_USED(MODULE_NANOCOAP_DTLS)
#include "net/credman.h"
#include "tinydtls_keys.h"

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = CONFIG_NANOCOAP_SOCK_DTLS_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif

/* _list_all_inet6() and _print_addr() derived from sc_gnrc_netif.c */
#ifdef MODULE_GNRC_IPV6
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

static int _cmd_inet6(int argc, char **argv)
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

SHELL_COMMAND(inet6, "IPv6 addresses", _cmd_inet6);
#endif

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("nanocoap test app");

#if IS_USED(MODULE_NANOCOAP_DTLS)
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        printf("nanocoap: cannot add credential to system: %d\n", res);
        return res;
    }
#endif

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
