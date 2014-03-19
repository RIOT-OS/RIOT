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
 * @file    rpl_routing_tables.h
 * @brief   Manages routing tables
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

#include <string.h>
#include "ipv6.h"
#include "forwarding_tables.h"

#ifndef ROUTING_TABLES_H_INCLUDED
#define ROUTING_TABLES_H_INCLUDED

// Init only when root of RPL-Dodag 
void init_routing_tables(uint16_t max_routing_entries);
void add_routing_entry(ipv6_addr_t *destination, ipv6_addr_t *parent_table);
void del_routing_entry(ipv6_addr_t *destination);
void calculate_routes();

// Returns array of intermediate hosts for a given destination
ipv6_addr_t *get_routing_info(ipv6_addr_t *destination);

#endif