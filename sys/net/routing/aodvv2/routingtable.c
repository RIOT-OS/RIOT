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

#include <stdio.h>
#include <inttypes.h>

#include "routingtable.h"
#include "aodv_debug.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* helper functions */
static void _reset_entry_if_stale(uint8_t i);
static void print_json_added_rt_entry(struct aodvv2_routing_entry_t *entry);

static struct aodvv2_routing_entry_t routing_table[AODVV2_MAX_ROUTING_ENTRIES];
static timex_t null_time, max_seqnum_lifetime, active_interval, max_idletime, validity_t;
timex_t now;
#if ENABLE_DEBUG
static struct netaddr_str nbuf;
#endif

void routingtable_init(void)
{
    null_time = timex_set(0, 0);
    max_seqnum_lifetime = timex_set(AODVV2_MAX_SEQNUM_LIFETIME, 0);
    active_interval = timex_set(AODVV2_ACTIVE_INTERVAL, 0);
    max_idletime = timex_set(AODVV2_MAX_IDLETIME, 0);
    validity_t = timex_set(AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME, 0);

    memset(&routing_table, 0, sizeof(routing_table));
    AODV_DEBUG("routing table initialized.\n");
}

struct netaddr *routingtable_get_next_hop(struct netaddr *dest, aodvv2_metric_t metricType)
{
    struct aodvv2_routing_entry_t *entry = routingtable_get_entry(dest, metricType);
    if (!entry) {
        return NULL;
    }
    return (&entry->nextHopAddr);
}

void routingtable_add_entry(struct aodvv2_routing_entry_t *entry)
{
    print_json_added_rt_entry(entry);

    /* only add if we don't already know the address */
    if (routingtable_get_entry(&(entry->addr), entry->metricType)) {
        return;
    }
    /*find free spot in RT and place rt_entry there */
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].addr._type == AF_UNSPEC) {
            memcpy(&routing_table[i], entry, sizeof(struct aodvv2_routing_entry_t));
            return;
        }
    }
}

struct aodvv2_routing_entry_t *routingtable_get_entry(struct netaddr *addr,
                                                      aodvv2_metric_t metricType)
{
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        _reset_entry_if_stale(i);

        if (!netaddr_cmp(&routing_table[i].addr, addr)
            && routing_table[i].metricType == metricType) {
            DEBUG("[routing] found entry for %s :", netaddr_to_string(&nbuf, addr));
#if ENABLE_DEBUG
            print_routingtable_entry(&routing_table[i]);
#endif
            return &routing_table[i];
        }
    }
    return NULL;
}

void routingtable_delete_entry(struct netaddr *addr, aodvv2_metric_t metricType)
{
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        _reset_entry_if_stale(i);

        if (!netaddr_cmp(&routing_table[i].addr, addr)
                && routing_table[i].metricType == metricType) {
            memset(&routing_table[i], 0, sizeof(routing_table[i]));
            return;
        }
    }
}

void routingtable_break_and_get_all_hopping_over(struct netaddr *hop,
                                                 struct unreachable_node unreachable_nodes[],
                                                 size_t *len)
{
    *len = 0; /* to be sure */

    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        _reset_entry_if_stale(i);

        if (netaddr_cmp(&routing_table[i].nextHopAddr, hop) == 0) {
            if (routing_table[i].state == ROUTE_STATE_ACTIVE &&
                    *len < AODVV2_MAX_UNREACHABLE_NODES) {
                /* when the max number of unreachable nodes is reached we're screwed.
                 * the above check is just damage control. */
                unreachable_nodes[*len].addr = routing_table[i].addr;
                unreachable_nodes[*len].seqnum = routing_table[i].seqnum;

                (*len)++;
                DEBUG("\t[routing] unreachable node found: %s\n", netaddr_to_string(&nbuf, &routing_table[i].nextHopAddr));
            }
            routing_table[i].state = ROUTE_STATE_INVALID;
            DEBUG("\t[routing] number of unreachable nodes: %i\n", *len);
        }
    }
}

/*
 * Check if entry at index i is stale as described in Section 6.3.
 * and clear the struct it fills if it is
 */
static void _reset_entry_if_stale(uint8_t i)
{
    vtimer_now(&now);
    timex_t lastUsed, expirationTime;

    if (timex_cmp(routing_table[i].expirationTime, null_time) == 0) {
        return;
    }

    int state = routing_table[i].state;
    lastUsed = routing_table[i].lastUsed;
    expirationTime = routing_table[i].expirationTime;

    /* an Active route is considered to remain Active as long as it is used at least once
     * during every ACTIVE_INTERVAL. When a route is no longer Active, it becomes an Idle route. */

    /* if the node is younger than the active interval, don't bother */
    if (timex_cmp(now, active_interval) < 0) {
        return;
    }

    if ((state == ROUTE_STATE_ACTIVE) &&
        (timex_cmp(timex_sub(now, active_interval), lastUsed) == 1)) {
        DEBUG("\t[routing] route towards %s Idle\n",
              netaddr_to_string(&nbuf, &routing_table[i].addr));
        routing_table[i].state = ROUTE_STATE_IDLE;
        routing_table[i].lastUsed = now; /* mark the time entry was set to Idle */
    }

    /* After an Idle route remains Idle for MAX_IDLETIME, it becomes an Invalid route. */

    /* if the node is younger than the expiration time, don't bother */
    if (timex_cmp(now, expirationTime) < 0) {
        return;
    }

    /* If Current_Time > Route.ExpirationTime, set Route.State := Invalid. */
    if ((state == ROUTE_STATE_IDLE) &&
        (timex_cmp(now, expirationTime) > 0)) {
        DEBUG("\t[routing] route towards %s became Invalid\n",
              netaddr_to_string(&nbuf, &routing_table[i].addr));
        routing_table[i].state = ROUTE_STATE_INVALID;
        routing_table[i].lastUsed = now; /* mark the time entry was set to Invalid */
    }

    /* If (Current_Time - Route.LastUsed) > (ACTIVE_INTERVAL + MAX_IDLETIME),
     * and if (Route.Timed == FALSE), set Route.State := Invalid. */
    if ((timex_cmp(timex_sub(now, lastUsed), timex_add(active_interval, max_idletime)) > 0) &&
        (state != ROUTE_STATE_TIMED)) {
        routing_table[i].state = ROUTE_STATE_INVALID;
    }

    /* After that time, old sequence number information is considered no longer
     * valid and the Invalid route MUST BE expunged */
    if (timex_cmp(timex_sub(now, lastUsed), max_seqnum_lifetime) >= 0) {
        DEBUG("\t[routing] Expunged routing table entry for %s at %i\n",
              netaddr_to_string(&nbuf, &routing_table[i].addr), i);
        memset(&routing_table[i], 0, sizeof(routing_table[i]));
    }
}

bool routingtable_offers_improvement(struct aodvv2_routing_entry_t *rt_entry,
                                     struct node_data *node_data)
{
    /* (TODO only guaranteed for AODVV2_DEFAULT_METRIC_TYPE!)*/
    bool is_loop_free = node_data->metric <= rt_entry->metric;
    int stale = seqnum_cmp(node_data->seqnum, rt_entry->seqnum);

    if ((stale == 1)                                                 /* New info is more recent and MUST be used */
        || ((stale == 0) && (node_data->metric < rt_entry->metric))  /* New info offers a better route and SHOULD be used */
        || ((stale == 0) && (node_data->metric >= rt_entry->metric)  /* Route is not an improvement, */
                         && (rt_entry->state == ROUTE_STATE_INVALID) /* but repairs an invalid route */
                         && is_loop_free)                             /* and contains no loops */
        ) {
        return true;
    }
    return false;
}

void routingtable_fill_routing_entry_t_rreq(struct aodvv2_packet_data *packet_data,
                                            struct aodvv2_routing_entry_t *rt_entry)
{
    rt_entry->addr = packet_data->origNode.addr;
    rt_entry->seqnum = packet_data->origNode.seqnum;
    rt_entry->nextHopAddr = packet_data->sender;
    rt_entry->lastUsed = packet_data->timestamp;
    rt_entry->expirationTime = timex_add(packet_data->timestamp, validity_t);
    rt_entry->metricType = packet_data->metricType;
    rt_entry->metric = packet_data->origNode.metric;
    rt_entry->state = ROUTE_STATE_ACTIVE;
}

void routingtable_fill_routing_entry_t_rrep(struct aodvv2_packet_data *packet_data,
                                            struct aodvv2_routing_entry_t *rt_entry)
{
    rt_entry->addr = packet_data->targNode.addr;
    rt_entry->seqnum = packet_data->targNode.seqnum;
    rt_entry->nextHopAddr = packet_data->sender;
    rt_entry->lastUsed = packet_data->timestamp;
    rt_entry->expirationTime = timex_add(packet_data->timestamp, validity_t);
    rt_entry->metricType = packet_data->metricType;
    rt_entry->metric = packet_data->targNode.metric;
    rt_entry->state = ROUTE_STATE_ACTIVE;
}

#if TEST_SETUP
/* Write JSON representation of rt_entry to json_str */
static void routingtable_entry_to_json(struct aodvv2_routing_entry_t *rt_entry, char* json_str)
{

    struct netaddr_str nbuf_addr, nbuf_nexthop;

    sprintf(json_str,"{\"addr\": \"%s\", \"next_hop\": \"%s\", \"seqnum\": %d,"
                     "\"metric\": %d, \"state\": %d}",
                     netaddr_to_string(&nbuf_addr, &rt_entry->addr),
                     netaddr_to_string(&nbuf_nexthop, &rt_entry->nextHopAddr),
                     rt_entry->seqnum, rt_entry->metric, rt_entry->state);
}
#endif

static void print_json_added_rt_entry(struct aodvv2_routing_entry_t *entry)
{
#if TEST_SETUP
    char rt_entry_json [500];
    routingtable_entry_to_json(entry, rt_entry_json);
    printf("{\"log_type\": \"added_rt_entry\", \"log_data\": %s}\n", rt_entry_json);
#else
    (void) entry; /* silence compiler */
#endif
}

void print_routingtable(void)
{
    printf("===== BEGIN ROUTING TABLE ===================\n");
    for (int i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        /* route has been used before => non-empty entry */
        if (routing_table[i].lastUsed.seconds
            || routing_table[i].lastUsed.microseconds) {
            print_routingtable_entry(&routing_table[i]);
        }
    }
    printf("===== END ROUTING TABLE =====================\n");
}

void print_routingtable_entry(struct aodvv2_routing_entry_t *rt_entry)
{
    struct netaddr_str nbuf;

    printf(".................................\n");
    printf("\t address: %s\n", netaddr_to_string(&nbuf, &(rt_entry->addr)));
    printf("\t seqnum: %i\n", rt_entry->seqnum);
    printf("\t nextHopAddress: %s\n",
            netaddr_to_string(&nbuf, &(rt_entry->nextHopAddr)));
    printf("\t lastUsed: %"PRIu32":%"PRIu32"\n",
            rt_entry->lastUsed.seconds, rt_entry->lastUsed.microseconds);
    printf("\t expirationTime: %"PRIu32":%"PRIu32"\n",
            rt_entry->expirationTime.seconds, rt_entry->expirationTime.microseconds);
    printf("\t metricType: %i\n", rt_entry->metricType);
    printf("\t metric: %d\n", rt_entry->metric);
    printf("\t state: %d\n", rt_entry->state);
}
