/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief Node application
 *
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "vtimer.h"
#include "thread.h"
#include "net_if.h"
#include "sixlowpan.h"
#include "udp.h"
#include "rpl.h"
#include "rpl/rpl_dodag.h"
#include "node.h"
#include "transceiver.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TRANSCEIVER TRANSCEIVER_DEFAULT

//char monitor_stack_buffer[MONITOR_STACK_SIZE];
radio_address_t id;

uint8_t is_root = 0;

void udp_init_node_standalone(void)//int argc, char **argv)
{
    transceiver_command_t tcmd;
    msg_t m;
    int32_t chan = RADIO_CHANNEL;

    //cpuid_get(&id);
    DEBUG("Setting HW address to %u\n", id);
    net_if_set_hardware_address(0, id);

    DEBUG("Initializing RPL for interface 0\n");
    uint8_t state = rpl_init(0);

    if (state != SIXLOWERROR_SUCCESS) {
        printf("Error initializing RPL\n");
    }
    else {
        puts("6LoWPAN and RPL initialized.");
    }

    ipv6_iface_set_routing_provider(rpl_get_next_hop);
    /* add global address */
    ipv6_addr_t tmp;
    /* initialize prefix */
    
    /* set host suffix */
    /* configure link-local address */
    ipv6_addr_set_link_local_prefix(&tmp);    
    if (!ipv6_addr_set_by_eui64(&tmp, 0, &tmp)) {
        DEBUG("Can not set link-local by EUI-64 on interface %d\n", 0);
        return;
    }
    if (!ipv6_net_if_add_addr(0, &tmp, NDP_ADDR_STATE_PREFERRED,
                NDP_OPT_PI_VLIFETIME_INFINITE,
                NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        DEBUG("Can not add link-local address to interface %d\n", 0);
        return;
    }
    //ipv6_net_if_add_addr(0, &tmp, NDP_ADDR_STATE_PREFERRED, 0, 0, 0);
    ipv6_addr_set_all_nodes_addr(&tmp);
    if (!ipv6_net_if_add_addr(0, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        DEBUG("Can not add all nodes address to interface %d\n", 0);
        return;
    }

    ipv6_addr_init(&tmp, 0xabcd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, id);
    ipv6_net_if_add_addr(0, &tmp, NDP_ADDR_STATE_PREFERRED, 0, 0, 0);
    /* set channel to 10 */
    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &chan;
    m.type = SET_CHANNEL; //SET
    m.content.ptr = (void *) &tcmd;

    msg_send_receive(&m, &m, transceiver_pid);
    printf("Channel set to %u\n", RADIO_CHANNEL);

    puts("Transport layer initialized");
    /* start transceiver watchdog */
}


