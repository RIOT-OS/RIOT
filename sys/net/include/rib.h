/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup
 * @brief       
 * @ingroup     net
 * @{
 *
 * @file        rib.h
 * @brief       Routing Information Base RIB
 *              The RIB ensures centralized handling of forwarding and routing.
 *              It manages a generalized routing base, using specific routing protocols to provide decision on the next_hop choice.
 *
 *              Roughly the operation can be sketched as follows:
 *              First the RIB has to be initialized using routing_init(max_entries) to provide a starting state
 *   
 *              It is assumed that a routing protocol implements a 'detached' callback function for next_hop decisions,
 *              e.g. rib_t *choose_nexthop(rib_t **aCandidates, size_t candidatesSize);
 *               It is called from here passing an array of candidates for routing.
 *              The provided array entries contain all metrics added by the routing protocol when adding a new route.
 *              Using the information from the list, the routing protocol selects one entry and returns it here.
 *
 *              - It is NOT recommended to use the provided pointers outside this callback. (And I mean it! Do not use them!)
 *   
 *              A protocol registers itself here providing a routing_handler_t with some information and the callback function pointer. 
 *              - Multiple protocols can be registered using route_register_protocol(...)
 *               (however handling multiple routing protocols is currently not possible, but prepared)
 *              - and the protocols can be removed using route_remove_protocol(protocol_id) 
 *
 *              The entries in the RIB are filled by the routing protocol using the provided management functions route_(add|del)(...)
 *              These functions dynamically allocate memory on _add and release it on _del completely
 *
 *              The callback function
 *              ipv6_addr_t *route_get_next_hop(ipv6_addr_t *dst_net_addr_in);
 *              is registered as ip_get_next_hop(...) to provide a next hop to the network_layer for a given destination
 *              The next hop address is stored here globally (by value) to avoid collisions accessing the returned pointer from a different context
 *
 *     
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#ifndef __RIB_H
#define __RIB_H

//#include <stddef.h>
#include "mutex.h"
#include "ipv6.h"

/*
Flags
Possible flags include
U (route is up)
H (target is a host)
G (use gateway)
R (reinstate route for dynamic routing)
D (dynamically installed by daemon or redirect)
M (modified from routing daemon or redirect)
A (installed by addrconf)
C (cache entry)
!(reject route)
*/
#define ROUTING_ROUTE_IS_UP (0x01)
#define ROUTING_ROUTE_IS_DEFAULT_GATEWAY (0x02)
#define ROUTING_TARGET_IS_HOST (0x04)
#define ROUTING_TARGET_IS_GATEWAY (0x08)
#define ROUTING_REJECT_ROUTE (0x0F)

// return values
#define ROUTING_RET_OK (0x0)
#define ROUTING_RET_ONLY_DEFAULT_GATEWAY (0x99)
#define ROUTING_RET_NO_ROUTE_FOUND -1

// specific value to indicate an unset metric value
#define ROUTING_METRIC_UNSET_VALUE (0xFFFFFFFF)

#define ROUTING_METRIC_ID_DEFAULT (0xFF)

// mutex for exclusive operations on the list
mutex_t mtx_table;

// default set value for a maximum number of entries persisting in the routing table
size_t max_entries;

// the current number of entries in the routing table
size_t current_entries;

typedef struct metric_t {
    struct metric_t *next;
    uint32_t       origin_protocol_id;
    uint8_t        met_id;
    uint32_t       met_val;
} metric_t;

typedef struct rib_t {
    struct rib_t    *next;
    ipv6_addr_t     destination;
    ipv6_addr_t     next_hop;
    uint8_t         flags;
    int             iface_id;
    metric_t       *metric;
} rib_t;

// pointer to the starting entry of the routing table
rib_t    *routing_table;

// pointer to quickly access the default gateway
rib_t    *default_gateway;

// the current next hop is stored here;
ipv6_addr_t     current_next_hop;

typedef struct routing_handler_t {
    struct routing_handler_t *next;
    uint32_t         protocol_id;
    metric_t   *supported_metric_id;
    rib_t *(* choose_nexthop)(rib_t **aCandidates, size_t candidatesSize);
} routing_handler_t;

// pointer to the starting registered routing protocol
routing_handler_t   *routing_protocols_registered;


/**
 * @brief   register routing protocol to use the OS routing table
 *          NOTE it is possible to register more than one protocol but only the first is used currently
 *
 * @param   protocol the protocol handler with information on the protocol and callback functions
 */
void route_register_protocol(routing_handler_t *protocol);

/**
 * @brief removes routing protocol
 *
 * @param protocol_id the id of the protocol to remove
 */
void route_remove_protocol(uint32_t protocol_id);

/**
 * @brief   Initialize routing table
 *
 * @param   max maximum allowed entries in the routing table
 *
 */
void route_init(size_t max);

/**
 * @brief   add an entry to the routing table
 *
 * @param   dst_net_addr pointer to the destination address
 * @param   gw_net_addr pointer to the next_hop used to reach the dst_net_addr
 * @param   iface_id pointer to the transceiver interface identifier
 * @param   metric pointer to the metric(s) used for this routing entry
 *
 */
void route_add(ipv6_addr_t *dst_net_addr, ipv6_addr_t *gw_net_addr, int *iface_id, metric_t *metric);

/**
 * @brief   remove all entries from the routing table matching the passed arguments
 *
 * @param   dst_net_addr pointer to the address to be deleted
 * @param   iface_id pointer to the transceiver interface identifier
 * @param   metric pointer to the metric(s) used for this routing entry
 *
 */
void route_del(ipv6_addr_t *dst_net_addr, int *iface_id);

/**
 * @brief   receive the default gateway
 *
 * @param   default_gw_net_addr_out pointer to an ipv6_addr_t to store the address
 * @param   iface_id_out pointer to the transceiver interface identifier used by the default gateway
 *
 */
void route_get_default_gateway(ipv6_addr_t *default_gw_net_addr_out, int *iface_id_out);

/**
 * @brief   collect all matching routing entries for the destination and ask the routing protocol to choose the next hop
 *
 * @param   dst_net_addr_in pointer to the destination address
 * @param   next_hop_out pointer to the structure holding the next_hop address (e.g. current_next_hop)
 *
 * @return  ROUTING_RET_OK if a specific route has been found
 *          ROUTING_RET_ONLY_DEFAULT_GATEWAY if only the default gateway can be used
 *          ROUTING_RET_NO_ROUTE_FOUND if there is no route to use for the destination
 */
int route_handle_candidates(ipv6_addr_t *dst_net_addr_in, ipv6_addr_t *next_hop_out);

/**
 * @brief returns the next hop for the given dst_net_addr_in
 *
 * @return the next hop address
 */
ipv6_addr_t *route_get_next_hop(ipv6_addr_t *dst_net_addr_in);

// Helper Functions

/**
 * @brief   prunes all entries and free all consumed memory
 *
 */
void prune_table();

/**
 * @brief   find all entries with longest common prefix and collects them
 *          The given address is searched in all entries to match either full for host or longest common entry for gateway routing
 *          DO NOT USE this function directly! It must be used with gained mtx_table to avoid unforeseen behavior.
 *
 * @param   dst_net_addr_in pointer to an ipv6_addr_t of the desired destination route
 * @param   aRIBentries array of rib_t pointer to store all matching entries
 * @param   aRIBentries_size_in_out pointer to the size variable providing the current size of aRIBentries and
 *          indicating the actual used size for the scribed entries on return
 *
 */
void find_longest_common_prefix(ipv6_addr_t *dst_net_addr_in, rib_t **aRIBentries, size_t *aRIBentries_size_in_out);

/**
 * @brief   find all metric entries matching the metric id from the target and store the values in the given query
 *
 * @param   metric_query_in_out pointer to the desired metric id(s) to be scribed
 * @param   metric_target pointer to an the target metrics to find the desired entries
 *
 * @erturn deviation from the given query and target entries
 */
int set_metric_values(metric_t *metric_query_in_out, metric_t *metric_target);


/**
 * @brief creates a list of 'count' empty metrics
 *
 * @param count the number of chained metrics
 *
 * @return pointer to the starting metric
 */
metric_t *create_empty_metrics(size_t count);


/**
 * @brief removes all entries and frees the used memory from the given starting entry
 *
 * @param starting_entry pointer to the first element to be deleted
 *
 */
void prune_metrics(metric_t *starting_entry);

void print_table();
void print_rib_t(rib_t *pRib);
void print_metric_t(metric_t *pMet);


/** @} */
#endif /* __RIB_H */
