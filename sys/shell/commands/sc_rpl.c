/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup shell_commands
 * @{
 * @file    sc_rpl.c
 * @brief   provides shell commands to manage and query RPL
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "rpl.h"

static char addr_str[IPV6_MAX_ADDR_STR_LEN];

void _rpl_route_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    rpl_routing_entry_t *rtable;
    rtable = rpl_get_routing_table();
    unsigned c = 0;
    puts("--------------------------------------------------------------------");
    puts("Routing table");
    printf(" %-3s  %-18s  %-18s  %s\n", "#", "target", "next hop", "lifetime");
    puts("--------------------------------------------------------------------");

    for (int i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rtable[i].used) {
            c++;
            printf(" %03d: %-18s  ", i, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                            (&rtable[i].address)));
            printf("%-18s  ", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                            (&rtable[i].next_hop)));
            printf("%d\n", rtable[i].lifetime);

        }
    }
    puts("--------------------------------------------------------------------");
    printf(" %u routing table entries\n", c);

    puts("$");
}
