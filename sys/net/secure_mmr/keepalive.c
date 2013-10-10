/*
 * keepalive.c - Sends keep-alive packets between nodes
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        keepalive.c
 * @brief       Sends keep-alive packets between nodes.
 *              Transfers keep-alives
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        08.09.2013 20:14:17
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/
/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


/* Encryption Modules */
#include "CBCMode.h"
#include "SkipJack.h"
#include "RC5.h"
#include "AES.h"
#include "TwoFish.h"
#include "3DES.h"

/* Project Includes */
#include "secure_routing_globals.h"
#include "network_security.h"
#include "keepalive.h"
#include "microeval.h"
#include "network_nodes.h"
#include "keyexchange.h"

/*---------------------------------------------------------------------------*/
//                                      Functions
/*---------------------------------------------------------------------------*/
static int packet_arrived(secure_packet_t *packet);
static int keepalive_timer_ticked(int user_param);
static int keepalive_arrived(secure_packet_t *packet);
static int start_keepalive_timer(void);

/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
static int keepalive_timer_id = -1;   // The id of the keep alive timer
static uint32_t number_keepalives_sent = 0;  // Number of KA sent by this node


/* Component public interface */
network_security_component_t keep_alive_component = {
    "KeepAlive",
    keepalive_init,
    keepalive_start,
    keepalive_stop,
    packet_arrived,
    0
};


/**
 * @brief Gets the keep-alive component
 *
 */
network_security_component_t *keep_alive_get_component()
{
    return &keep_alive_component;
}

/**
 * @brief Inits the keep-alive component
 *
 */
int keepalive_init()
{
    ulog("> Keep alive component inited");
    number_keepalives_sent = 0;

    return NetworkSecurity_OK;
}

/**
 * @brief Starts the keep-alive component
 *
 */
int keepalive_start()
{
    ulog("> Keep-Alive start. Registering keep-alive timer. Tick freq=%ds",
         KEEPALIVE_INTERVAL);

    start_keepalive_timer();

    return NetworkSecurity_OK;
}

/**
 * @brief Stoarts timer for keepalive
 *
 */
int start_keepalive_timer(void)
{
    uint32_t timer_interval = KEEPALIVE_INTERVAL + (rand() % 60);

    if (keepalive_timer_id != -1) {
        network_security_restart_timer(keepalive_timer_id, timer_interval);
    }
    else
        keepalive_timer_id = network_security_register_timer(
                                 timer_interval,
                                 0,
                                 keepalive_timer_ticked);

    return NetworkSecurity_OK;
}

/**
 * @brief Stops the keep-alive component
 *
 */
int keepalive_stop()
{
    network_security_stop_timer(keepalive_timer_id);

    return NetworkSecurity_OK;
}

/**
 * @brief Called when the keep-alive timer is to tick
 *
 */
int keepalive_timer_ticked(int user_param)
{
    ulog("!Keep-Alive timer ticked");

    broadcast_keepalive();

    return NetworkSecurity_Continue;
}

/**
 * @brief Called to send out a keep-alive broadcast
 *
 */
int broadcast_keepalive()
{
    ulog("Broadcasting keep-alive");

    // Check if any neighbours. If not, send no-neighbours message
    if (network_nodes_get_any_neighbour(network_nodes_get_root()) == 0) {
        ulog("> No neighbours in table");
        // network_security_raise_event(NetworkSecurity_NoNeighbours, 0);
        // return NetworkSecurity_Continue;
    }

    ulog("> Incrementing missed-keep-alive of neighbours");
    // Increase the keepalive of all neighbours
    network_nodes_increment_neighbours_keepalive(network_nodes_get_root());


    // Get rid everyone who has too many missing keepalive
    network_nodes_drop_dead_neighbours(network_nodes_get_root());

    if (network_nodes_count_neighbours(network_nodes_get_root()) == 0) {
        ulog("> Queueing a hello message because no neighbours");
        network_security_raise_event(NetworkSecurity_NoNeighbours, 0);
    }

    // Send broadcast keep-alive message

    number_keepalives_sent++;

    ulog("> Sending keep-alive message via local broadcast");
    keepalive_msg_t msg;
    msg.type = NetworkSecurity_KeepAlive;
    msg.gkey_version = keyexchange_get_key_seq_nmbr();
    return network_security_send_packet((uint8_t *)&msg,
                                        sizeof(keepalive_msg_t),
                                        LOCAL_BROADCAST,
                                        1,
                                        SecurityMode_MAC, PeekHint_DontPeek
                                       );
}

/**
 * @brief Called when a packet arrives. The component looks if it is one of
 *                our packets
 *
 * @param packet The secure packet
 *
 * @return The result of the operation as networksecurity message
 */
int packet_arrived(secure_packet_t *packet)
{
    if (packet->data[0] == NetworkSecurity_KeepAlive) {
        ulog("> Keep-alive packet arrived");
        keepalive_arrived(packet);
        return NetworkSecurity_PacketHandled;
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Print out internal info about keep alive component
 */
void keepalive_print_info()
{
    if (keepalive_timer_id > -1) {
        printf("Keep-Alive timer currently enabled. Interval=%ds\n",
               KEEPALIVE_INTERVAL);
    }
    else {
        printf("Keep-Alive timer currently disabled\n");
    }

    printf("Sent %lu Keep-Alives so far\n", number_keepalives_sent);
}

/**
 * @brief Called when a keep-alive packet arrives
 *
 * @param packet The secure packet
 *
 * @return The result of the operation as networksecurity message
 */
int keepalive_arrived(secure_packet_t *packet)
{
    // find the node

    // Reset wrongmacs and missed keepalives for thenode
    network_node_t *node = network_nodes_find(packet->source);

    if (node) {
        ulog("> Node %d found. Missed Keepalive from %d->0",
             node->node_id,
             node->missed_keep_alives);
        node->wrong_MACs = 0;
        node->missed_keep_alives = 0;

        if (node->is_neighbour == 0) {
            ulog("> Node was not a neighbour");
            network_security_raise_event(NetworkSecurity_NeighbourDetected,
                                         node->node_id);
        }
    }
    else {
        // If node is not found in the table, then start a hello sequence for
        // this node Pass a message to network_security with the id of this
        // node, and "unknown node detected" message

        ulog("> Keep-Alive node not found in neighbours");
        network_security_raise_event(NetworkSecurity_NeighbourDetected,
                                     node->node_id);
    }

    return NetworkSecurity_OK;
}
