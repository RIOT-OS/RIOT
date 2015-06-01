/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2015 Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
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
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>

#include "udp.h"
#include "thread.h"

#include "aodvv2/aodvv2.h"

#include "aodv_writer_tests.h"
#include "aodv_fib_tests.h"
#include "aodv_tests.h"

#define AODVV2_IFACE           (0)      /**< The used Trasmssion device */

/** The node IPv6 address */
ipv6_addr_t myaddr;

radio_address_t aodvv2_iface_id;
struct netaddr aodvv2_test_sender_oa, aodvv2_test_sender_ta, aodvv2_test_origaddr, aodvv2_test_targaddr;

/**
* @brief prepares this node
* @return 0 on success
*/
static int aodvv2_setup_node(void)
{
    /* setup the radio interface */
    if( net_if_set_src_address_mode(AODVV2_IFACE, NET_IF_TRANS_ADDR_M_SHORT) != 1 ) {
        return -1;
    }

    aodvv2_iface_id = net_if_get_hardware_address(AODVV2_IFACE);
    if( aodvv2_iface_id == 0 ) {
        return -1;
    }

    /* choose addresses */
    ipv6_addr_init(&myaddr, 0x2015, 0x3, 0x18, 0x1111, 0x0, 0x0, 0x0, aodvv2_iface_id);

    /* and set it */
    if( ipv6_net_if_add_addr(AODVV2_IFACE, &myaddr, NDP_ADDR_STATE_PREFERRED, 0, 0, 0) != 1) {
        return -1;
    }

    return 0;
}

/**
* @brief init data that needs to be globally known
*/
static int aodvv2_init_testdata(void)
{
    if( netaddr_from_string(&aodvv2_test_origaddr, "::10") == -1 ) {
        return -1;
    }

    if( netaddr_from_string(&aodvv2_test_sender_oa, "::11") == -1 ) {
        return -1;
    }

    if( netaddr_from_string(&aodvv2_test_sender_ta, "::12") == -1 ) {
        return -1;
    }

    if( netaddr_from_string(&aodvv2_test_targaddr, "::13") == -1 ) {
        return -1;
    }
    return 0;
}

void aodv_test_drop_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length)
{
    (void) buffer;
    (void) length;
}

int main(void)
{
    if( aodvv2_init_testdata() != 0 ) {
        return -1;
    }

    if( aodvv2_setup_node() != 0 ) {
        return -1;
    }

    aodv_init();

    write_packets_to_buf();

    sleep(5);

    /* TODO:
    - use route and see if it updates the lifetime
    */
    aodv_test_add_to_fib();

    return 0;
}
