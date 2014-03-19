/**
 * RPL dodag implementation
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    forwarding_tables.c
 * @brief   Manages forwarding tables
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

#include "forwarding_tables.h"

routing_entry_t *forwarding_table;
uint8_t forwarding_max_entries;

/* helper functions */

void clear_forwarding_table() {
    for (uint8_t i = 0; i < forwarding_max_entries; i++) {
        memset(&forwarding_table[i], 0, sizeof(forwarding_table[i]));
    }
}

bool equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2)
{
    for (uint8_t i = 0; i < 4; i++) {
        if (id1->uint32[i] != id2->uint32[i]) {
            return false;
        }
    }

    return true;

}

/* core functions */

void forwarding_table_init(uint8_t max_entries) {

    forwarding_max_entries = max_entries;
	forwarding_table = malloc(sizeof(routing_entry_t)*forwarding_max_entries);
    clear_forwarding_table();

}

void add_forwarding_entry(ipv6_addr_t *next_hop, ipv6_addr_t *destination, uint16_t lifetime) {

    routing_entry_t *entry = find_forwarding_entry(destination);

    if (entry != NULL) {
        entry->lifetime = lifetime;
        return;
    }

    for (uint8_t i = 0; i < forwarding_max_entries; i++) {
        if (!forwarding_table[i].used) {
            forwarding_table[i].destination = *destination;
            forwarding_table[i].next_hop = *next_hop;
            forwarding_table[i].lifetime = lifetime;
            forwarding_table[i].used = 1;
            break;
        }
    }
}

routing_entry_t *find_forwarding_entry(ipv6_addr_t *destination)
{
    for (uint8_t i = 0; i < forwarding_max_entries; i++) {
        if (forwarding_table[i].used && equal_id(&forwarding_table[i].destination, destination)) {
            return &forwarding_table[i];
        }
    }

    return NULL;
}

routing_entry_t *get_forwarding_table(void)
{
    return forwarding_table;
}

void del_routing_entry(ipv6_addr_t *destination)
{
    for (uint8_t i = 0; i < forwarding_max_entries; i++) {
        if (forwarding_table[i].used && equal_id(&forwarding_table[i].destination, destination)) {
            memset(&forwarding_table[i], 0, sizeof(forwarding_table[i]));
            return;
        }
    }
}

ipv6_addr_t *get_next_hop(ipv6_addr_t *dest)
{
    for (uint8_t i = 0; i < forwarding_max_entries; i++) {
        if (forwarding_table[i].used && equal_id(&forwarding_table[i].destination, dest)) {
            return &forwarding_table[i].next_hop;
        }
    }

    return NULL;
}

