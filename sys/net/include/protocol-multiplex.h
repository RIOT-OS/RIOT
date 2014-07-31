/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
 * and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @defgroup    net_mmstack Protocol multiplex
 * @ingroup     net
 * @brief       Protocol handler multiplexing
 * @{
 *
 * @file
 * @brief       Protocol handler multiplexing
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 */

#ifndef PROTOCOLMULTIPLEX_H_
#define PROTOCOLMULTIPLEX_H_


#include "radio/types.h"

typedef struct {
    packet_handler_t    handler;
    protocol_t          protocol;
} handler_entry_t;

typedef struct {
    uint8_t             size;
    handler_entry_t *handler;
} pm_table_t;

void pm_init_table(pm_table_t *table, uint8_t size, handler_entry_t *handler);
int pm_find_handler_index(const pm_table_t *table, protocol_t protocol, unsigned int start);
int pm_set_handler(const pm_table_t *table, protocol_t protocol, packet_handler_t handler);
void pm_remove_handler(const pm_table_t *table, protocol_t protocol, packet_handler_t handler);
int pm_invoke(const pm_table_t *table, protocol_t protocol, void *payload, int payload_size, packet_info_t *packet_info);

/** @} */
#endif /* PROTOCOLMULTIPLEX_H_ */
