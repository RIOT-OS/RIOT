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
 * @brief UDP RPL example application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
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
#include "rpl_udp.h"
#include "transceiver.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TRANSCEIVER TRANSCEIVER_DEFAULT

char monitor_stack_buffer[MONITOR_STACK_SIZE];
radio_address_t id;

uint8_t is_root = 0;

void rpl_udp_init(int argc, char **argv)
{
    transceiver_command_t tcmd;
    msg_t m;
    uint8_t chan = RADIO_CHANNEL;

    if (argc != 2) {
        printf("Usage: %s (r|n|h)\n", argv[0]);
        printf("\tr\tinitialize as root\n");
        printf("\tn\tinitialize as node router\n");
        printf("\th\tinitialize as non-routing node (host-mode)\n");
        return;
    }

    char command = argv[1][0];
    if ((command == 'n') || (command == 'r') || (command == 'h')) {
        printf("INFO: Initialize as %srouting %s on address %d\n",
               ((command == 'h') ? "non-" : ""),
               (((command == 'n') || (command == 'h')) ? "node" : "root"), id);

#if (defined(MODULE_CC110X) || defined(MODULE_CC110X_LEGACY) || defined(MODULE_CC110X_LEGACY_CSMA))
        if (!id || (id > 255)) {
            printf("ERROR: address not a valid 8 bit integer\n");
            return;
        }
#endif

        DEBUGF("Setting HW address to %u\n", id);
        net_if_set_hardware_address(0, id);

        if (command != 'h') {
            DEBUGF("Initializing RPL for interface 0\n");
            uint8_t state = rpl_init(0);

            if (state != SIXLOWERROR_SUCCESS) {
                printf("Error initializing RPL\n");
            }
            else {
                puts("6LoWPAN and RPL initialized.");
            }

            if (command == 'r') {
                rpl_init_root();
                ipv6_iface_set_routing_provider(rpl_get_next_hop);
                is_root = 1;
            }
            else {
                ipv6_iface_set_routing_provider(rpl_get_next_hop);
            }
        }
        else {
            puts("6LoWPAN initialized.");
        }

        DEBUGF("Start monitor\n");
        kernel_pid_t monitor_pid = thread_create(monitor_stack_buffer,
                                                 sizeof(monitor_stack_buffer),
                                                 PRIORITY_MAIN - 2,
                                                 CREATE_STACKTEST,
                                                 rpl_udp_monitor,
                                                 NULL,
                                                 "monitor");
        DEBUGF("Register at transceiver %02X\n", TRANSCEIVER);
        transceiver_register(TRANSCEIVER, monitor_pid);
        ipv6_register_packet_handler(monitor_pid);
        //sixlowpan_lowpan_register(monitor_pid);
    }
    else {
        printf("ERROR: Unknown command '%c'\n", command);
        return;
    }

    /* add global address */
    ipv6_addr_t tmp;
    /* initialize prefix */
    ipv6_addr_init(&tmp, 0xabcd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, id);
    /* set host suffix */
    ipv6_addr_set_by_eui64(&tmp, 0, &tmp);
    ipv6_net_if_add_addr(0, &tmp, NDP_ADDR_STATE_PREFERRED, 0, 0, 0);

    if (command != 'h') {
        ipv6_init_as_router();
    }

    /* set channel to 10 */
    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &chan;
    m.type = SET_CHANNEL;
    m.content.ptr = (void *) &tcmd;

    msg_send_receive(&m, &m, transceiver_pid);
    printf("Channel set to %u\n", RADIO_CHANNEL);

    puts("Transport layer initialized");
    /* start transceiver watchdog */
}

void rpl_udp_dodag(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("---------------------------\n");
    rpl_dodag_t *mydodag = rpl_get_my_dodag();

    if (mydodag == NULL) {
        printf("Not part of a dodag\n");
        printf("---------------------------\n");
        return;
    }

    printf("Part of Dodag:\n");
    printf("%s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                    (&mydodag->dodag_id)));
    printf("my rank: %d\n", mydodag->my_rank);

    if (!is_root) {
        printf("my preferred parent:\n");
        printf("%s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                        (&mydodag->my_preferred_parent->addr)));
    }

    printf("---------------------------\n");
}
