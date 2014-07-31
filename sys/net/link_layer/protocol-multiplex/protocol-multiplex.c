/******************************************************************************
 * Copyright 2009-2010, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @addtogroup  net_mmstack
 * @{
 */

/**
 * @file
 * @internal
 * @brief       Protocol handler multiplexing
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @version     $Revision: 1938 $
 *
 * @note        $Id: protocol-multiplex.c 1938 2010-03-02 12:14:29Z baar $
 */

#include <stddef.h>
#include <string.h>
#include "protocol-multiplex.h"

//#define DEBUG
#ifdef DEBUG
#include <stdio.h>
#define PRINTF(fmt,args...) printf("pm: " fmt "\n", ##args)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
void pm_init_table(pm_table_t *table, uint8_t size, handler_entry_t *handler)
{
    table->size = size;
    table->handler = handler;
    memset(handler, 0, sizeof(handler_entry_t) * size);
}
/*---------------------------------------------------------------------------*/
int pm_find_handler_index(const pm_table_t *table, protocol_t protocol,
                          unsigned int start)
{
    int i;
    handler_entry_t *e = &table->handler[start];

    for (i = start; i < table->size; i++, e++) {
        if (e->protocol == protocol) {
            return i;
        }
    }

    return -1;
}
/*---------------------------------------------------------------------------*/
int pm_set_handler(const pm_table_t *table, protocol_t protocol,
                   packet_handler_t handler)
{
    /* Reject illegal values */
    if (protocol == 0 || handler == NULL) {
        PRINTF("proto %u rejected", protocol);
        return -1;
    }

    /* Check if there is already a handler for given protocol */
    int index = pm_find_handler_index(table, protocol, 0);

    if (index >= 0) {
        /* Two handlers for same protocol not allowed because only
         * one gets called. This hasn't to be the last one who
         * registered!*/
        PRINTF("proto %u handler found, reset", protocol);
        table->handler[index].protocol = 0;
        table->handler[index].handler = NULL;
    }

    /* Find free position for handler (protocol value is 0) */
    index = pm_find_handler_index(table, 0, 0);

    /* Store handler if free index found */
    if (index >= 0) {
        PRINTF("proto %u, set", protocol);
        table->handler[index].protocol = protocol;
        table->handler[index].handler = handler;
    }

    /* Return result (-1 on error or no free index) */
    return index;
}
/*---------------------------------------------------------------------------*/
void pm_remove_handler(const pm_table_t *table, protocol_t protocol,
                       packet_handler_t handler)
{
    int i;

    for (i = 0; i < table->size; i++) {
        if (table->handler[i].protocol == protocol && table->handler[i].handler == handler) {
            PRINTF("proto %u handler found, reset", protocol);
            table->handler[i].protocol = 0;
            table->handler[i].handler = NULL;
        }
    }
}
/*---------------------------------------------------------------------------*/
int pm_invoke(const pm_table_t *table, protocol_t protocol, void *payload,
              int payload_size, packet_info_t *packet_info)
{
    int index = 0;

    /* Reject illegal values */
    if (protocol == 0) {
        return -1;
    }

    if ((index = pm_find_handler_index(table, protocol, index)) != -1) {
        PRINTF("proto %u, invoke", protocol);
        table->handler[index].handler(payload, payload_size, packet_info);
    }
    else {
        PRINTF("proto %u invoke failed (no handler)", protocol);
    }

    return index;
}
/*---------------------------------------------------------------------------*/
/** @} */
