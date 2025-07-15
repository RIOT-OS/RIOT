/*
 * Copyright (C) 2025 Ahmad Mahmod <ahmad.nawras.mahmod@gmail.com -- mahmod@unistra.fr> *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>
#include "net/gnrc/rpl/sr_table.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/ipv6.h"
#include "net/ipv6/addr.h"

#define ENABLE_DEBUG  0
#include "debug.h"

static fib_table_t table;
static fib_entry_t _sr_table_entries[GNRC_SR_FIB_TABLE_SIZE];

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
static ipv6_addr_t root_node = { { 0 } };

void gnrc_sr_initialize_table(ipv6_addr_t *root, kernel_pid_t iface)
{
    (void)iface;
    if (root != NULL) {
        memcpy(&root_node, root, sizeof(ipv6_addr_t));
    }
    else {
        DEBUG("FIB Table not Initialized, invalid root address\n");
    }

    table.size = GNRC_SR_FIB_TABLE_SIZE;
    table.table_type = FIB_TABLE_TYPE_SH;
    table.data.entries = _sr_table_entries;

    fib_init(&table);
    DEBUG("FIB Table Initialized\n");

}

void gnrc_sr_table_print(void)
{
    DEBUG("FIB Table Content:\n");
    fib_print_routes(&table);
}

void gnrc_sr_print_table_route(ipv6_addr_t *node)
{
    /* Initialize route retrieve */
    ipv6_addr_t route_buffer[GNRC_SR_MAX_ROUTE_SIZE];
    size_t route_length = 0;
    /* Retrieve the route from the specified node to the root node */
    int result = gnrc_sr_get_full_route(node, route_buffer, &route_length);

    printf("A route to %s route has been found: ",
           ipv6_addr_to_str(addr_str, (ipv6_addr_t *)node, sizeof(addr_str)));
    if (result >= 0) {
        for (size_t i = 0; i < route_length; ++i) {
            printf(" %s - ",
                   ipv6_addr_to_str(addr_str, &route_buffer[i], sizeof(addr_str)));
        }
        printf("\n");
    }
    else {
        printf("No route found");
    }
}

int gnrc_sr_add_new_dst(ipv6_addr_t *child, size_t prefix, ipv6_addr_t *parent,
                        kernel_pid_t sr_iface_id,
                        uint32_t sr_flags, uint32_t lifetime)
{
    /* Initialize a new SR */
    if (child == NULL || parent == NULL
        || ipv6_addr_is_unspecified(child) || ipv6_addr_is_unspecified(parent)
        || (prefix == 0) || (sr_flags != FIB_FLAG_RPL_ROUTE)
        || ipv6_addr_equal(child, parent)) {
        DEBUG("Invalid Parameter\n");
        return -1;
    }

    DEBUG("SR Table: Child %s - ", ipv6_addr_to_str(addr_str, (ipv6_addr_t *)child,
                                                    sizeof(addr_str)));
    DEBUG("Parent: %s\n", ipv6_addr_to_str(addr_str, (ipv6_addr_t *)parent, sizeof(addr_str)));

    if (fib_add_entry(&table, sr_iface_id, (uint8_t *)child, prefix, sr_flags,
                      (uint8_t *)parent, prefix, sr_flags, lifetime * 1000) == 0) {
        /* Append only the child-parent pair */
        DEBUG("A new entry has been added to SR table.\n");
        return 0;
    }
    else {
        DEBUG("Failed to create source route for child-parent pair.\n");
        return -1;
    }
}

int gnrc_sr_delete_route(ipv6_addr_t *dst_node, size_t dst_size)
{
    if (dst_node == NULL) {
        DEBUG("Invalid IPv6 Address\n");
        return -1;
    }
    fib_remove_entry(&table,  (uint8_t *)dst_node, dst_size);
    DEBUG("Route to destination deleted successfully.\n");

    return 0;
}

int gnrc_sr_get_full_route(ipv6_addr_t *dst_node, ipv6_addr_t *route_buffer,
                           size_t *route_length)
{
    if (dst_node == NULL || route_buffer == NULL || route_length == NULL) {
        DEBUG("Invalid parameter\n");
        return -1;
    }

    ipv6_addr_t current_node;
    memcpy(&current_node, dst_node, sizeof(ipv6_addr_t));

    size_t route_index = 0;

    while (memcmp(&current_node, &root_node, sizeof(ipv6_addr_t)) != 0) {
        kernel_pid_t sr_iface_id = 0;
        ipv6_addr_t next_hop;
        size_t next_hop_size = sizeof(ipv6_addr_t);
        uint32_t next_hop_flags = FIB_FLAG_RPL_ROUTE;
        uint32_t sr_flags = FIB_FLAG_RPL_ROUTE;

        /* Get the next hop (parent) for the current node */
        int result = fib_get_next_hop(&table, &sr_iface_id, (uint8_t *)&next_hop, &next_hop_size, \
                                      &next_hop_flags, (uint8_t *)&current_node,
                                      sizeof(ipv6_addr_t), sr_flags);

        if (result < 0) {
            DEBUG("Result = %d - Failed to find route to the root. Stuck at node: %s\n", result,
                  ipv6_addr_to_str(addr_str, (ipv6_addr_t *)&current_node, sizeof(addr_str)));
            return -1;
        }
        memcpy(&route_buffer[(route_index)], &current_node, sizeof(ipv6_addr_t));
        route_index++;

        if (memcmp(&current_node, &next_hop, sizeof(ipv6_addr_t)) != 0) {
            /* Move to the parent */
            memcpy(&current_node, &next_hop, sizeof(ipv6_addr_t));
        }
        else {
            break;
        }
    }

    *route_length = route_index;

    if (ENABLE_DEBUG) {
        DEBUG("Route reconstructed: ");
        for (size_t i = 0; i < *route_length; ++i) {
            DEBUG("%s", ipv6_addr_to_str(addr_str, (ipv6_addr_t *)&route_buffer[i],
                                         sizeof(addr_str)));
            if (i < *route_length - 1) {
                DEBUG(" -> ");
            }
        }
        DEBUG(".\n");
    }
    return 0;
}
