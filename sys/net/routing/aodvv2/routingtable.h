/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file
 * @brief       Cobbled-together routing table.
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_ROUTINGTABLE_H_
#define AODVV2_ROUTINGTABLE_H_

#include <string.h>

#include "common/netaddr.h"

#include "aodvv2/types.h"
#include "constants.h"
#include "seqnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A route table entry (i.e., a route) may be in one of the following states:
 */
enum aodvv2_routing_states
{
    ROUTE_STATE_ACTIVE,
    ROUTE_STATE_IDLE,
    ROUTE_STATE_INVALID,
    ROUTE_STATE_TIMED
};

/**
 * all fields of a routing table entry
 */
struct aodvv2_routing_entry_t
{
    struct netaddr addr;                /**< IP address of this route's destination */
    aodvv2_seqnum_t seqnum;             /**< The Sequence Number obtained from the
                                         *   last packet that updated the entry */
    struct netaddr nextHopAddr;         /**< IP address of the the next hop towards
                                         *   the destination */
    timex_t lastUsed;                   /**< IP address of this route's destination */
    timex_t expirationTime;             /**< Time at which this route expires */
    aodvv2_metric_t metricType;         /**< Metric type of this route */
    uint8_t metric;                     /**< Metric value of this route*/
    uint8_t state;                      /**< State of this route
                                         *   (i.e. one of aodvv2_routing_states) */
};

/**
 * @brief     Initialize routing table.
 */
void routingtable_init(void);

/**
 * @brief     Get next hop towards dest.
 *            Returns NULL if dest is not in routing table.
 *
 * @param[in] dest        Destination of the packet
 * @param[in] metricType  Metric Type of the desired route
 * @return                next hop towards dest if it exists, NULL otherwise
 */
struct netaddr *routingtable_get_next_hop(struct netaddr *dest, aodvv2_metric_t metricType);

/**
 * @brief     Add new entry to routing table, if there is no other entry
 *            to the same destination.
 *
 * @param[in] entry        The routing table entry to add
 */
void routingtable_add_entry(struct aodvv2_routing_entry_t *entry);

/**
 * @brief     Retrieve pointer to a routing table entry.
 *            To edit, simply follow the pointer.
 *            Returns NULL if addr is not in routing table.
 *
 * @param[in] addr          The address towards which the route should point
 * @param[in] metricType    Metric Type of the desired route
 * @return                  Routing table entry if it exists, NULL otherwise
 */
struct aodvv2_routing_entry_t *routingtable_get_entry(struct netaddr *addr, aodvv2_metric_t metricType);

/**
 * @brief     Delete routing table entry towards addr with metric type MetricType,
 *            if it exists.
 *
 * @param[in] addr          The address towards which the route should point
 * @param[in] metricType    Metric Type of the desired route
 */
void routingtable_delete_entry(struct netaddr *addr, aodvv2_metric_t metricType);

/**
 * Find all routing table entries that use hop as their nextHopAddress, mark them
 * as broken, write the active one into unreachable_nodes[] and increment len
 * accordingly. (Sorry about the Name.)
 *
 * @param hop                 Address of the newly unreachable next hop
 * @param unreachable_nodes[] array of newlu unreachable nodes to be filled.
 *                            should be empty.
 * @param len                 size_t* which will contain the length of
 *                            unreachable_nodes[] after execution
 */
void routingtable_break_and_get_all_hopping_over(struct netaddr *hop,
                                                 struct unreachable_node unreachable_nodes[],
                                                 size_t *len);

/**
 * Check if the data of a RREQ or RREP offers improvement for an existing routing
 * table entry.
 * @param rt_entry            the routing table entry to check
 * @param node_data           The data to check against. When handling a RREQ,
 *                            the OrigNode's information (i.e. packet_data.origNode)
 *                            must be passed. When handling a RREP, the
 *                            TargNode's data (i.e. packet_data.targNode) must
 *                            be passed.
 */
bool routingtable_offers_improvement(struct aodvv2_routing_entry_t *rt_entry,
                                     struct node_data *node_data);

/**
 * Fills a routing table entry with the data of a RREQ.
 * @param packet_data         the RREQ's data
 * @param rt_entry            the routing table entry to fill
 * @param link_cost           the link cost for this RREQ
 */
void routingtable_fill_routing_entry_t_rreq(struct aodvv2_packet_data *packet_data,
                                            struct aodvv2_routing_entry_t *rt_entry);

/**
 * Fills a routing table entry with the data of a RREP.
 * @param packet_data         the RREP's data
 * @param rt_entry            the routing table entry to fill
 * @param link_cost           the link cost for this RREP
 */
void routingtable_fill_routing_entry_t_rrep(struct aodvv2_packet_data *packet_data,
                                            struct aodvv2_routing_entry_t *rt_entry);

void print_routingtable(void);
void print_routingtable_entry(struct aodvv2_routing_entry_t *rt_entry);

#ifdef __cplusplus
}
#endif

#endif /* AODVV2_ROUTINGTABLE_H_*/
