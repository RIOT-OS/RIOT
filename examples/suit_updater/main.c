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
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "firmware.h"
#include "shell.h"
#include "msg.h"
#include "suit.h"
#include "net/ota_suit.h"
#include "firmware/manifest.h"
#include "xtimer.h"

#include "net/ipv6/addr.h"

#include "ps.h"

#define MAIN_QUEUE_SIZE     (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void print_iface_addresses(kernel_pid_t iface);

static void print_interfaces_addresses(void)
{
    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        print_iface_addresses(netif->pid);
    }
}

static void print_iface_addresses(kernel_pid_t iface)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    int res = gnrc_netapi_get(iface, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                          sizeof(ipv6_addrs));

    if (res < 0) {
        return;
    }
    uint8_t ipv6_addrs_flags[GNRC_NETIF_IPV6_ADDRS_NUMOF];

    memset(ipv6_addrs_flags, 0, sizeof(ipv6_addrs_flags));
    /* assume it to succeed (otherwise array will stay 0) */
    gnrc_netapi_get(iface, NETOPT_IPV6_ADDR_FLAGS, 0, ipv6_addrs_flags,
                    sizeof(ipv6_addrs_flags));

    for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
        /* print only global addresses */
        if (ipv6_addr_is_link_local(&ipv6_addrs[i])) {
            continue;
        }
        ipv6_addr_to_str(addr_str, &ipv6_addrs[i], sizeof(addr_str));
        printf("iface %u inet6 addr: %s\n", iface, addr_str);
    }
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT suit OTA example application");
    printf("Running from slot %i\n", firmware_current_slot());
    suit_uuid_init();

    firmware_manifest_run();
    ota_suit_run();

    /* start shell */
    puts("All up and running");
    while(1) {
        xtimer_sleep(10);
        ps();
        print_interfaces_addresses();
    }

    /* should be never reached */
    return 0;
}
