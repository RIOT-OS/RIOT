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
 * @file    rpl_routing_tables.c
 * @brief   Manages routing tables
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

 #include "rpl_routing_tables.h"

ipv6_addr_t **routing_table_entries;