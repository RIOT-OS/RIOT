/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "net/fib.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"
#include "net/netdev2.h"
#include "net/netopt.h"

#include "net/uhcp.h"
#include "log.h"
#include "fmt.h"

static kernel_pid_t gnrc_border_interface;
static kernel_pid_t gnrc_wireless_interface;

static void set_interface_roles(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);

    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
        kernel_pid_t dev = ifs[i];
        int is_wired = gnrc_netapi_get(dev, NETOPT_IS_WIRED, 0, NULL, 0);
        if ((!gnrc_border_interface) && (is_wired == 1)) {
            ipv6_addr_t addr, defroute;
            gnrc_border_interface = dev;

            ipv6_addr_from_str(&addr, "fe80::2");
            gnrc_ipv6_netif_add_addr(dev, &addr, 64,
                                     GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST);

            ipv6_addr_from_str(&defroute, "::");
            ipv6_addr_from_str(&addr, "fe80::1");
            fib_add_entry(&gnrc_ipv6_fib_table, dev, defroute.u8, 16,
                    0x00, addr.u8, 16, 0,
                    (uint32_t)FIB_LIFETIME_NO_EXPIRE);
        }
        else if ((!gnrc_wireless_interface) && (is_wired != 1)) {
            gnrc_wireless_interface = dev;
        }

        if (gnrc_border_interface && gnrc_wireless_interface) {
            break;
        }
    }

    LOG_INFO("gnrc_uhcpc: Using %u as border interface and %u as wireless interface.\n", gnrc_border_interface, gnrc_wireless_interface);
}

static ipv6_addr_t _prefix;

void uhcp_handle_prefix(uint8_t *prefix, uint8_t prefix_len, uint16_t lifetime, uint8_t *src, uhcp_iface_t iface)
{
    (void)prefix_len;
    (void)lifetime;
    (void)src;

    eui64_t iid;
    if (!gnrc_wireless_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix, but don't know any wireless interface\n");
        return;
    }

    if ((kernel_pid_t)iface != gnrc_border_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix from unexpected interface\n");
        return;
    }

    if (gnrc_netapi_get(gnrc_wireless_interface, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid((ipv6_addr_t*)prefix, iid.uint8);
    }
    else {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): cannot get IID of wireless interface\n");
        return;
    }

    if (ipv6_addr_equal(&_prefix, (ipv6_addr_t*)prefix)) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): got same prefix again\n");
        return;
    }

    gnrc_ipv6_netif_add_addr(gnrc_wireless_interface, (ipv6_addr_t*)prefix, 64,
                             GNRC_IPV6_NETIF_ADDR_FLAGS_UNICAST |
                             GNRC_IPV6_NETIF_ADDR_FLAGS_NDP_AUTO);

    gnrc_ipv6_netif_remove_addr(gnrc_wireless_interface, &_prefix);
    print_str("gnrc_uhcpc: uhcp_handle_prefix(): configured new prefix ");
    ipv6_addr_print((ipv6_addr_t*)prefix);
    puts("/64");

    if (!ipv6_addr_is_unspecified(&_prefix)) {
        gnrc_ipv6_netif_remove_addr(gnrc_wireless_interface, &_prefix);
        print_str("gnrc_uhcpc: uhcp_handle_prefix(): removed old prefix ");
        ipv6_addr_print(&_prefix);
        puts("/64");
    }

    memcpy(&_prefix, prefix, 16);
}

extern void uhcp_client(uhcp_iface_t iface);

static char _uhcp_client_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
static msg_t _uhcp_msg_queue[4];

static void* uhcp_client_thread(void *arg)
{
    (void)arg;

    msg_init_queue(_uhcp_msg_queue, sizeof(_uhcp_msg_queue)/sizeof(msg_t));
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
