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

#ifndef UNASSIGNED_CHANNEL
#define UNASSIGNED_CHANNEL INT_MIN
#endif

#define TRANSCEIVER TRANSCEIVER_DEFAULT

static char monitor_stack_buffer[MONITOR_STACK_SIZE];
radio_address_t id;

static uint8_t is_root = 0;

void rpl_udp_init(int argc, char **argv)
{
    transceiver_command_t tcmd;
    msg_t m;
    int32_t chan = UNASSIGNED_CHANNEL;

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

        tcmd.transceivers = TRANSCEIVER;
        tcmd.data = &chan;
        m.type = GET_CHANNEL;
        m.content.ptr = (void *) &tcmd;

        msg_send_receive(&m, &m, transceiver_pid);
        if( chan == UNASSIGNED_CHANNEL ) {
            DEBUGF("The channel has not been set yet.");

            /* try to set the channel to 10 (RADIO_CHANNEL) */
            chan = RADIO_CHANNEL;
        }

        m.type = SET_CHANNEL;
        msg_send_receive(&m, &m, transceiver_pid);
        if( chan == UNASSIGNED_CHANNEL ) {
            puts("ERROR: channel NOT set! Aborting initialization.");
            return;
        }

        printf("Channel set to %" PRIi32 "\n", chan);

        /* global address */
        ipv6_addr_t global_addr, global_prefix;
        ipv6_addr_init(&global_prefix, 0xabcd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
        ipv6_addr_set_by_eui64(&global_addr, 0, &global_prefix);

        if (command != 'h') {
            DEBUGF("Initializing RPL for interface 0\n");

            uint8_t state = SIXLOWERROR_VALUE;
            if (command == 'n') {
                /*
                 * no global address specified, we'll use auto address config
                 * initiated by the root node
                 */
                state = rpl_init(0, NULL);
            }
            else if (command == 'r') {
                rpl_options_t rpl_opts = {
                    .instance_id = 0,
                    .prefix = global_prefix,
                    .prefix_len = 64,
                    .prefix_flags = RPL_PREFIX_INFO_AUTO_ADDR_CONF,
                    /* autonomous address-configuration */
                };

                /* use specific global address */
                state = rpl_init(0, &global_addr);
                rpl_init_root(&rpl_opts);
                is_root = 1;
            }

            if (state != SIXLOWERROR_SUCCESS) {
                puts("Error initializing RPL");
            }
            else {
                puts("6LoWPAN and RPL initialized.");
            }

            ipv6_iface_set_routing_provider(rpl_get_next_hop);

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

    if (command != 'h') {
        ipv6_init_as_router();
    }

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
