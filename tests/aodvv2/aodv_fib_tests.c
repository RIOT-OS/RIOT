/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2015 Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       tests for the interaction between AODVv2 and the FIB
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>

#include "aodv_fib_tests.h"
#include "aodv_tests.h"

#include "common/autobuf.h"
#include "rfc5444/rfc5444_writer.h"
#include "rfc5444/rfc5444_print.h"
#include "ng_fib.h"

#include "reader.h"
#include "utils.h"


/* make sure packet is not rejected because "sender" is not in the neighbor cache */
static void _aodv_test_add_to_nc(struct netaddr *sender_na){
    ipv6_addr_t sender;

    netaddr_to_ipv6_addr_t(sender_na, &sender);
    /* TODO: isrouter = 1 correct? */
    ndp_neighbor_cache_add(aodvv2_iface_id, &sender, &sender.uint16[7], 2, 1,
                        NDP_NCE_STATUS_REACHABLE, NDP_NCE_TYPE_TENTATIVE, 0xffff);
}

/* Handle RREQ with no anomalies */
static void aodv_test_add_to_fib_regular_rreq(void)
{
    printf("\n============= Handling regular RREQ ================================\n");
    ipv6_addr_t next_hop;
    kernel_pid_t iface_id;
    size_t next_hop_size = sizeof(ipv6_addr_t);
    uint32_t next_hop_flags = 0;

    _aodv_test_add_to_nc(aodv_test_plain_rreq.sender);
    aodv_packet_reader_handle_packet((void *) aodv_test_plain_rreq.buffer,
                                     aodv_test_plain_rreq.length,
                                     aodv_test_plain_rreq.sender);

    printf("Checking FIB ...\n");

    /* Check if route back to origaddr was created */
    int fib_success = fib_get_next_hop(&iface_id, &next_hop.uint8[0], &next_hop_size,
                                       &next_hop_flags, aodvv2_test_origaddr._addr,
                                       sizeof(ipv6_addr_t), 0);
    assert(0 == fib_success);

    printf("Done.\n");
}

/* Handle RREQ with no anomalies */
static void aodv_test_add_to_fib_regular_rrep(void)
{
    printf("\n============= Handling regular RREP ================================\n");
    ipv6_addr_t next_hop;
    kernel_pid_t iface_id;
    size_t next_hop_size = sizeof(ipv6_addr_t);
    uint32_t next_hop_flags = 0;
    timex_t now;

    _aodv_test_add_to_nc(aodv_test_plain_rrep.sender);

    /* Make sure route back is known TODO make this global too?!*/
    vtimer_now(&now);
    struct aodvv2_routing_entry_t tmp_rt_entry = {
        .addr = aodvv2_test_origaddr,
        .seqnum = 1,
        .nextHopAddr = aodvv2_test_sender_oa,
        .lastUsed = now,
        .expirationTime = timex_add(now,
                                    timex_set(AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME, 0)),
        .metricType = AODVV2_DEFAULT_METRIC_TYPE,
        .metric = 2,
        .state = ROUTE_STATE_ACTIVE,
    };
    routingtable_add_entry(&tmp_rt_entry);

    aodv_packet_reader_handle_packet((void *) aodv_test_plain_rrep.buffer,
                                     aodv_test_plain_rrep.length,
                                     aodv_test_plain_rrep.sender);
    printf("Checking FIB ...\n");

    /* Check if route back to origaddr was created */
    int fib_success = fib_get_next_hop(&iface_id, &next_hop.uint8[0], &next_hop_size,
                                       &next_hop_flags, aodvv2_test_targaddr._addr,
                                       sizeof(ipv6_addr_t), 0);
    assert(0 == fib_success);
    printf("Done.\n");
}

static void aodv_test_update_fib_regular_rreq(void)
{
    printf("\n============= Handling more recent RREQ ============================\n");
    timex_t lifetime, now;

    printf("Checking FIB ...\n");
    aodv_packet_reader_handle_packet((void *) aodv_test_more_recent_rreq.buffer,
                                     aodv_test_more_recent_rreq.length,
                                     aodv_test_more_recent_rreq.sender);

    assert(0 == fib_devel_get_lifetime(&lifetime, aodvv2_test_origaddr._addr, sizeof(ipv6_addr_t)));

    /* assuming some ms passed during these operations... */
    vtimer_now(&now);
    timex_t cmp_lifetime = timex_add(now, timex_set(0, 900000));
    assert(1 == timex_cmp(lifetime, cmp_lifetime));

    printf("Done. \n");
}

static void aodv_test_update_fib_regular_rrep(void)
{
    printf("\n============= Handling more recent RREP ============================\n");
    timex_t lifetime, now;

    printf("Checking FIB ...\n");
    aodv_packet_reader_handle_packet((void *) aodv_test_more_recent_rrep.buffer,
                                     aodv_test_more_recent_rrep.length,
                                     aodv_test_more_recent_rrep.sender);

    assert(0 == fib_devel_get_lifetime(&lifetime, aodvv2_test_targaddr._addr, sizeof(ipv6_addr_t)));

    /* assuming some ms passed during these operations... */
    vtimer_now(&now);
    timex_t cmp_lifetime = timex_add(now, timex_set(0, 900000));
    assert(1 == timex_cmp(lifetime, cmp_lifetime));
    printf("Done.\n");
}

static void aodv_test_route_expired(void)
{
    printf("\n============= testing if route vanishes after expiring ===========\n");
    kernel_pid_t iface_id;
    uint32_t next_hop_flags = 0;
    size_t next_hop_size = sizeof(ipv6_addr_t);
    ipv6_addr_t next_hop;

    printf("waiting until route expires... (about 4.5 minutes)\n");
    /* TODO: use MAXTIME */
    sleep(AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME);
    printf("Checking FIB ...\n");

    /* Check if route back to origaddr was created */
    int fib_success = fib_get_next_hop(&iface_id, &next_hop.uint8[0], &next_hop_size,
                                       &next_hop_flags, aodvv2_test_origaddr._addr,
                                       sizeof(ipv6_addr_t), 0);
    assert( 0 != fib_success);

    printf("Done.\n");

}

void aodv_test_add_to_fib(void)
{
    /* overwrite the aodvv2 packet writer so that messages aren't actually swnt */
    aodv_packet_writer_init(aodv_test_drop_packet);

    printf("Starting tests...\n");
    aodv_test_add_to_fib_regular_rreq();
    sleep(5);
    aodv_test_update_fib_regular_rreq();
    sleep(5);
    aodv_test_add_to_fib_regular_rrep();
    sleep(5);
    aodv_test_update_fib_regular_rrep();
    sleep(5);
    aodv_test_route_expired();
    printf("All done!\n");
}
