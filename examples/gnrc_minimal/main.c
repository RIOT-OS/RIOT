/*
 * Copyright (C) 2015 Inria
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
 * @brief       Showing minimum memory footprint of gnrc network stack
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6/netif.h"

int main(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];

    puts("RIOT network stack example application");

    /* get the first IPv6 interface and prints its address */
    size_t numof = gnrc_netif_get(ifs);
    if (numof > 0) {
        gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(ifs[0]);
        for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
            if ((ipv6_addr_is_link_local(&entry->addrs[i].addr)) && !(entry->addrs[i].flags & GNRC_IPV6_NETIF_ADDR_FLAGS_NON_UNICAST)) {
                char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
                ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, IPV6_ADDR_MAX_STR_LEN);
                printf("My address is %s\n", ipv6_addr);
            }
        }
    }

    /* main thread exits */
    return 0;
}
