/*
 * Copyright (C) 2016 Lotte Steenbrink
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include "embUnit.h"

#include "xtimer.h"

#include "routingtable.h"

#include "tests-gnrc_aodvv2.h"

/* test data */
struct netaddr addr_1, addr_2;
timex_t ts;

aodvv2_seqnum_t test_seqnum_1 = 1;
aodvv2_seqnum_t test_seqnum_2 = 2;
uint8_t test_metric_1 = 3;
uint8_t test_metric_2 = 4;

/* test basic routingtable add & get functions */
static void test_routingtable_add_get(void)
{
    struct aodvv2_routing_entry_t* entry;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);

    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(entry != NULL);
}

/* test if 2 entries are added correctly */
static void test_routingtable_add_2(void)
{
    struct aodvv2_routing_entry_t* entry_1;
    struct aodvv2_routing_entry_t* entry_2;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);
    routingtable_add_entry(&addr_2, &test_seqnum_2, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_2,
                           ROUTE_STATE_ACTIVE, &ts);

    entry_1 = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    entry_2 = routingtable_get_entry(&addr_2, AODVV2_DEFAULT_METRIC_TYPE);

    TEST_ASSERT(0 == netaddr_cmp(&entry_1->addr, &addr_1));
    TEST_ASSERT(entry_1->seqnum == test_seqnum_1);
    TEST_ASSERT(0 == netaddr_cmp(&entry_1->nextHopAddr, &addr_2));
    TEST_ASSERT(entry_1->metricType == AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(entry_1->metric == test_metric_1);
    TEST_ASSERT(entry_1->state == ROUTE_STATE_IDLE);

    TEST_ASSERT(0 == netaddr_cmp(&entry_2->addr, &addr_2));
    TEST_ASSERT(entry_2->seqnum == test_seqnum_2);
    TEST_ASSERT(0 == netaddr_cmp(&entry_2->nextHopAddr, &addr_2));
    TEST_ASSERT(entry_2->metricType == AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(entry_2->metric == test_metric_2);
    TEST_ASSERT(entry_2->state == ROUTE_STATE_ACTIVE);
}

static void test_routingtable_get_next_hop(void)
{
    struct netaddr* next_hop;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);

    next_hop = routingtable_get_next_hop(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(0 == netaddr_cmp(next_hop, &addr_2));
}

static void test_routingtable_update_entry(void)
{
    struct aodvv2_routing_entry_t* entry;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);
    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);

    entry = routingtable_update_entry(entry, &test_seqnum_2, &addr_1, test_metric_2,
                                      ROUTE_STATE_ACTIVE, &ts);

    TEST_ASSERT(entry->seqnum == test_seqnum_2);
    TEST_ASSERT(0 == netaddr_cmp(&entry->nextHopAddr, &addr_1));
    TEST_ASSERT(entry->metric == test_metric_2);
    TEST_ASSERT(entry->state == ROUTE_STATE_ACTIVE);
}

static void test_routingtable_delete_entry(void)
{
    struct aodvv2_routing_entry_t* entry;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);

    routingtable_delete_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);

    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(entry == NULL);
}

static void test_routingtable_break_and_get_all_hopping_over(void)
{
    struct unreachable_node unreachable_nodes[2];
    size_t len;
    struct aodvv2_routing_entry_t* entry_1;
    struct aodvv2_routing_entry_t* entry_2;

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);
    routingtable_add_entry(&addr_2, &test_seqnum_2, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_2,
                           ROUTE_STATE_ACTIVE, &ts);

    routingtable_break_and_get_all_hopping_over(&addr_2, unreachable_nodes, &len);

    entry_1 = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    entry_2 = routingtable_get_entry(&addr_2, AODVV2_DEFAULT_METRIC_TYPE);

    TEST_ASSERT(len == 1);

    /* The information from entry_1 shouldn't be in unreachable_nodes[], because
       routes with state ROUTE_STATE_IDLE should not be reported in a RERR (unless
       ENABLE_IDLE_IN_RERR is configured, which it isn't.) */
    TEST_ASSERT(entry_1->state == ROUTE_STATE_INVALID);

    TEST_ASSERT(0 == netaddr_cmp(&(unreachable_nodes[0].addr), &addr_2));
    TEST_ASSERT(unreachable_nodes[0].seqnum == test_seqnum_2);
    TEST_ASSERT(entry_2->state == ROUTE_STATE_INVALID);
}

static void test_routingtable_offers_improvement__seqnum(void)
{
    struct aodvv2_routing_entry_t* entry;
    struct node_data new_data =
    {
        .addr = addr_1,
        .metric = test_metric_1,
        .seqnum = test_seqnum_2,
    };

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);

    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(routingtable_offers_improvement(entry, &new_data) == true);
}

static void test_routingtable_offers_improvement__metric(void)
{
    struct aodvv2_routing_entry_t* entry;
    struct node_data new_data =
    {
        .addr = addr_1,
        .metric = test_metric_1,
        .seqnum = test_seqnum_1,
    };

    routingtable_add_entry(&addr_1, &test_seqnum_1, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_2,
                           ROUTE_STATE_IDLE, &ts);
    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);

    TEST_ASSERT(routingtable_offers_improvement(entry, &new_data) == true);
}

static void test_routingtable_offers_improvement__loopFree(void)
{
    struct aodvv2_routing_entry_t* entry;
    struct node_data new_data =
    {
        .addr = addr_1,
        .metric = test_metric_1,
        .seqnum = test_seqnum_1,
    };

    routingtable_add_entry(&addr_1, &test_seqnum_2, &addr_2,
                           AODVV2_DEFAULT_METRIC_TYPE, test_metric_1,
                           ROUTE_STATE_IDLE, &ts);

    entry = routingtable_get_entry(&addr_1, AODVV2_DEFAULT_METRIC_TYPE);
    TEST_ASSERT(routingtable_offers_improvement(entry, &new_data) == false);
}


static void setUp(void)
{
    xtimer_now_timex(&ts);

    /* init test data */
    int foo = netaddr_from_string(&addr_1, "::23");
    int bar = netaddr_from_string(&addr_2, "::42");
    (void) foo; /* silence compiler*/
    (void) bar; /* silence compiler*/

    /* init routing table */
    routingtable_init();
}

Test *tests_gnrc_aodvv2_table_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_routingtable_add_get),
        new_TestFixture(test_routingtable_add_2),
        new_TestFixture(test_routingtable_get_next_hop),
        new_TestFixture(test_routingtable_update_entry),
        new_TestFixture(test_routingtable_delete_entry),
        new_TestFixture(test_routingtable_break_and_get_all_hopping_over),
        new_TestFixture(test_routingtable_offers_improvement__seqnum),
        new_TestFixture(test_routingtable_offers_improvement__metric),
        new_TestFixture(test_routingtable_offers_improvement__loopFree),
    };

    EMB_UNIT_TESTCALLER(gnrc_aodvv2_tests, setUp, NULL, fixtures);

    return (Test *)&gnrc_aodvv2_tests;
}