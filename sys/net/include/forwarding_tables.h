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
 * @file    forwarding_tables.h
 * @brief   Header for forwarding tables
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

#include <string.h>
#include "ipv6.h"

#ifndef FORWARDING_TABLES_H_INCLUDED
#define FORWARDING_TABLES_H_INCLUDED
/* structs */ 

typedef struct {
    uint8_t used;
    ipv6_addr_t destination;
    ipv6_addr_t next_hop;
    uint16_t lifetime;

} routing_entry_t;

/* functions and variables */

/* Maximum routing entries is depending on routing algorithm */
void forwarding_table_init(uint8_t max_entries);
void add_forwarding_entry(ipv6_addr_t *next_hop, ipv6_addr_t *destination, uint16_t lifetime);
void del_forwarding_entry(ipv6_addr_t *destination);
ipv6_addr_t *get_next_hop(ipv6_addr_t *destination);
routing_entry_t *find_forwarding_entry(ipv6_addr_t *destination);
routing_entry_t *get_forwarding_table(void);
void clear_forwarding_table();

#endif

