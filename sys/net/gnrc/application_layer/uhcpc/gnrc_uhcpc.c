/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <kernel_defines.h>

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/rpl.h"
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"

#include "net/uhcp.h"
#include "log.h"
#include "fmt.h"

static kernel_pid_t gnrc_border_interface;
static kernel_pid_t gnrc_wireless_interface;

static void set_interface_roles(void)
{
    gnrc_netif_t *netif = NULL;

    while ((netif = gnrc_netif_iter(netif))) {
        kernel_pid_t dev = netif->pid;
        int is_wired = gnrc_netapi_get(dev, NETOPT_IS_WIRED, 0, NULL, 0);
        if ((!gnrc_border_interface) && (is_wired == 1)) {
            ipv6_addr_t addr, defroute = IPV6_ADDR_UNSPECIFIED;
            gnrc_border_interface = dev;

            ipv6_addr_from_str(&addr, "fe80::2");
            gnrc_netapi_set(dev, NETOPT_IPV6_ADDR, 64 << 8, &addr,
                            sizeof(addr));
            ipv6_addr_from_str(&addr, "fe80::1");
            gnrc_ipv6_nib_ft_add(&defroute, IPV6_ADDR_BIT_LEN, &addr, dev, 0);

            /* Disable router advertisements on upstream interface. With this,
             * the border router
             * 1. Does not confuse the upstream router to add the border router
             *    to its default router list and
             * 2. Solicits upstream Router Advertisements quicker to
             *    auto-configure its upstream global address.
             */
            gnrc_ipv6_nib_change_rtr_adv_iface(netif, false);
        }
        else if ((!gnrc_wireless_interface) && (is_wired != 1)) {
            gnrc_wireless_interface = dev;
        }

        if (gnrc_border_interface && gnrc_wireless_interface) {
            break;
        }
    }

    LOG_INFO("gnrc_uhcpc: Using %u as border interface and %u as wireless "
             "interface.\n", gnrc_border_interface, gnrc_wireless_interface);
}

void uhcp_handle_prefix(uint8_t *prefix, uint8_t prefix_len, uint16_t lifetime,
                        uint8_t *src, uhcp_iface_t iface)
{
    int idx;
    gnrc_netif_t *wireless;
    (void)src;

    if (!gnrc_wireless_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix, but "
                    "don't know any wireless interface\n");
        return;
    }

    if ((kernel_pid_t)iface != gnrc_border_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix from "
                    "unexpected interface\n");
        return;
    }

    wireless = gnrc_netif_get_by_pid(gnrc_wireless_interface);
    idx = gnrc_netif_ipv6_add_prefix(wireless, (ipv6_addr_t *)prefix, prefix_len,
                                     lifetime, lifetime);
    if (idx >= 0) {
        gnrc_rpl_configure_root(wireless, &wireless->ipv6.addrs[idx]);
    }
}

extern void uhcp_client(uhcp_iface_t iface);

static char _uhcp_client_stack[THREAD_STACKSIZE_DEFAULT +
                               THREAD_EXTRA_STACKSIZE_PRINTF];
static msg_t _uhcp_msg_queue[4];

static void* uhcp_client_thread(void *arg)
{
    (void)arg;

    msg_init_queue(_uhcp_msg_queue, ARRAY_SIZE(_uhcp_msg_queue));
    uhcp_client(gnrc_border_interface);
    return NULL;
}

void auto_init_gnrc_uhcpc(void)
{
    set_interface_roles();

    /* only start client if more than one interface is given */
    if (! (gnrc_border_interface && gnrc_wireless_interface)) {
        LOG_WARNING("gnrc_uhcpc: only one interface found, skipping setup.\n");
        return;
    }

    /* initiate uhcp client */
    thread_create(_uhcp_client_stack, sizeof(_uhcp_client_stack),
            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
            uhcp_client_thread, NULL, "uhcp");
}
