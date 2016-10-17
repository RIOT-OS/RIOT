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

#include "xtimer.h"
#include "timex.h"

#include "routingtable.h"
#include "aodv_debug.h"

#include "common/netaddr.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* helper functions */
static void _reset_entry_if_stale(uint8_t i);
static void print_json_added_rt_entry(struct aodvv2_routing_entry_t *entry);

static struct aodvv2_routing_entry_t routingtable[AODVV2_MAX_ROUTING_ENTRIES];
static timex_t null_time, max_seqnum_lifetime, active_interval, max_idletime, validity_t;
#if ENABLE_DEBUG
static struct netaddr_str nbuf;
#endif

timex_t now;

void routingtable_init(void)
{
    null_time = timex_set(0, 0);
    max_seqnum_lifetime = timex_set(AODVV2_MAX_SEQNUM_LIFETIME, 0);
    active_interval = timex_set(AODVV2_ACTIVE_INTERVAL, 0);
    max_idletime = timex_set(AODVV2_MAX_IDLETIME, 0);
    validity_t = timex_set(AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME, 0);

    memset(&routingtable, 0, sizeof(routingtable));
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

void routingtable_add_entry(struct netaddr *addr, aodvv2_seqnum_t *seqnum,
                            struct netaddr *nextHopAddr, aodvv2_metric_t metricType,
                            uint8_t metric, uint8_t state, timex_t *timestamp)
{
    /* only add if we don't already know the address */
    if (routingtable_get_entry(addr, metricType)) {
        /* stop compiler from complaining about unused entries
        (TODO am I shooting myself in the foot here?)*/
        (void) seqnum;
        (void) nextHopAddr;
        (void) metric;
        (void) state;
        (void) timestamp;
        return;
    }
    /*find free spot in RT and place rt_entry there */
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        if (routingtable[i].addr._type == AF_UNSPEC) {
            // TODO: is the memcpy here correct or do I need another & ?
            memcpy(&(routingtable[i].addr), addr, sizeof(struct netaddr));
            (routingtable[i].metricType) = metricType;
            routingtable_update_entry(&routingtable[i], seqnum, nextHopAddr,
                                      metric, state, timestamp);

            print_json_added_rt_entry(&routingtable[i]);
            return;
        }
    }
}

struct aodvv2_routing_entry_t *routingtable_get_entry(struct netaddr *addr,
                                                      aodvv2_metric_t metricType)
{
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        _reset_entry_if_stale(i);

        if (!netaddr_cmp(&routingtable[i].addr, addr)
            && routingtable[i].metricType == metricType) {
            DEBUG("[routing] found entry for %s :", netaddr_to_string(&nbuf, addr));
#if ENABLE_DEBUG
            print_routingtable_entry(&routingtable[i]);
#endif
            return &routingtable[i];
        }
    }
    return NULL;
}

struct aodvv2_routing_entry_t *routingtable_update_entry(
                            struct aodvv2_routing_entry_t *entry,
                            aodvv2_seqnum_t *seqnum, struct netaddr *nextHopAddr,
                            uint8_t metric, uint8_t state, timex_t *timestamp)
{
    entry->seqnum = *seqnum;
    memcpy(&entry->nextHopAddr, nextHopAddr, sizeof(struct netaddr));
    memcpy(&entry->lastUsed, timestamp, sizeof(timex_t));
    entry->expirationTime = timex_add(*timestamp, validity_t);
    entry->metric = metric;
    entry->state = state;
    return entry;
}

void routingtable_delete_entry(struct netaddr *addr, aodvv2_metric_t metricType)
{
    for (unsigned i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        _reset_entry_if_stale(i);

        if (!netaddr_cmp(&routingtable[i].addr, addr)
                && routingtable[i].metricType == metricType) {
            memset(&routingtable[i], 0, sizeof(routingtable[i]));
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

        if (netaddr_cmp(&routingtable[i].nextHopAddr, hop) == 0) {
            if (routingtable[i].state == ROUTE_STATE_ACTIVE &&
                    *len < AODVV2_MAX_UNREACHABLE_NODES) {
                /* when the max number of unreachable nodes is reached we're screwed.
                 * the above check is just damage control. */
                unreachable_nodes[*len].addr = routingtable[i].addr;
                unreachable_nodes[*len].seqnum = routingtable[i].seqnum;

                (*len)++;

                DEBUG("\t[routing] unreachable node found: %s\n", netaddr_to_string(&nbuf, &routingtable[i].nextHopAddr));
            }

            routingtable[i].state = ROUTE_STATE_INVALID;
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
    xtimer_now_timex(&now);
    timex_t lastUsed, expirationTime;

    if (timex_cmp(routingtable[i].expirationTime, null_time) == 0) {
        return;
    }

    int state = routingtable[i].state;
    lastUsed = routingtable[i].lastUsed;
    expirationTime = routingtable[i].expirationTime;

    /* an Active route is considered to remain Active as long as it is used at least once
     * during every ACTIVE_INTERVAL. When a route is no longer Active, it becomes an Idle route. */

    /* if the node is younger than the active interval, don't bother */
    if (timex_cmp(now, active_interval) < 0) {
        return;
    }

    if ((state == ROUTE_STATE_ACTIVE) &&
        (timex_cmp(timex_sub(now, active_interval), lastUsed) == 1)) {
        DEBUG("\t[routing] route towards %s Idle\n",
              netaddr_to_string(&nbuf, &routingtable[i].addr));
        routingtable[i].state = ROUTE_STATE_IDLE;
        routingtable[i].lastUsed = now; /* mark the time entry was set to Idle */
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
              netaddr_to_string(&nbuf, &routingtable[i].addr));
        routingtable[i].state = ROUTE_STATE_INVALID;
        routingtable[i].lastUsed = now; /* mark the time entry was set to Invalid */
    }

    /* If (Current_Time - Route.LastUsed) > (ACTIVE_INTERVAL + MAX_IDLETIME),
     * and if (Route.Timed == FALSE), set Route.State := Invalid. */
    if ((timex_cmp(timex_sub(now, lastUsed), timex_add(active_interval, max_idletime)) > 0) &&
        (state != ROUTE_STATE_TIMED)) {
        routingtable[i].state = ROUTE_STATE_INVALID;
    }

    /* After that time, old sequence number information is considered no longer
     * valid and the Invalid route MUST BE expunged */
    if (timex_cmp(timex_sub(now, lastUsed), max_seqnum_lifetime) >= 0) {
        DEBUG("\t[routing] Expunged routing table entry for %s at %i\n",
              netaddr_to_string(&nbuf, &routingtable[i].addr), i);
        memset(&routingtable[i], 0, sizeof(routingtable[i]));
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

// TODO: add ifdef DEBUGs or something
void print_routingtable(void)
{
    printf("===== BEGIN ROUTING TABLE ===================\n");
    for (int i = 0; i < AODVV2_MAX_ROUTING_ENTRIES; i++) {
        /* route has been used before => non-empty entry */
        if (routingtable[i].lastUsed.seconds
            || routingtable[i].lastUsed.microseconds) {
            print_routingtable_entry(&routingtable[i]);
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
