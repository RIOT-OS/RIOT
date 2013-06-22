/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#ifndef PROTOCOLMULTIPLEX_H_
#define PROTOCOLMULTIPLEX_H_

/**
 * @addtogroup	net
 * @{
 */

/**
 * @file
 * @internal
 * @brief		Protocol handler multiplexing
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @author		Michael Baar <baar@inf.fu-berlin.de>
 * @version     $Revision: 1526 $
 *
 * @note		$Id: protocol-multiplex.h 1526 2009-10-30 13:40:20Z hillebra $
 */

#include "radio/types.h"

typedef struct {
    packet_handler_t	handler;
    protocol_t			protocol;
} handler_entry_t;

typedef struct {
    uint8_t				size;
    handler_entry_t	*handler;
} pm_table_t;

void pm_init_table(pm_table_t *table, uint8_t size, handler_entry_t *handler);
int pm_find_handler_index(const pm_table_t *table, protocol_t protocol, unsigned int start);
int pm_set_handler(const pm_table_t *table, protocol_t protocol, packet_handler_t handler);
void pm_remove_handler(const pm_table_t *table, protocol_t protocol, packet_handler_t handler);
int pm_invoke(const pm_table_t *table, protocol_t protocol, void *payload, int payload_size, packet_info_t *packet_info);

/** @} */

#endif /* PROTOCOLMULTIPLEX_H_ */
