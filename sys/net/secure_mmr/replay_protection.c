/*
 * replayprotection.c - Protection against replayed packets.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        replayprotection.c
 * @brief       Protects against replayed packets
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        15.09.2013 17:35:35
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

/* Project Includes */
#include "secure_routing_globals.h"
#include "packet_security.h"
#include "network_nodes.h"
#include "microeval/microeval.h"
#include "replay_protection.h"
#include "network_security.h"

/* Encryption Includes */
#include "SkipJack.h"
#include "RC5.h"

/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/

static uint32_t sequence_nmbr = 0;
static uint8_t  is_in_soft_overflow = 0;

/* Component public interface */
packet_security_component_t replay_component = {
    "ReplayProtection",
    replay_protection_init_component,
    replay_protection_secure_packet,
    replay_protection_verify_packet,
    0
};


/**
 * Returns the replay component
 *
 * @return the replay component
 */
packet_security_component_t *replay_protection_get_component()
{
    return &replay_component;
}

/**
 * Initialises the replay component
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_init_component()
{
    replay_protection_reset_sequence_number();

    return PacketSecurity_OK;
}

/**
 * @brief Examines a packet and indicates if it passes the verification rules
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_verify_packet(secure_packet_t *packet, uint16_t phy_src)
{
    network_node_t *node = network_nodes_find(packet->source);

    if (node == 0) {
        // means we don't know this node
        ulog("> Node seen for first time. Storing");
        network_nodes_set_sequence_nmbr(packet->source, packet->seq_nmbr);
        return PacketSecurity_OK;
    }

    ulog("> Replay verifying packet. Src=%d and Seq=%lu. NodeSeq=%lu",
         packet->source, packet->seq_nmbr, node->seq_nmbr);

    uint32_t packet_sequence_nmbr = (uint32_t)packet->seq_nmbr;
    uint32_t node_sequence_nmbr = (uint32_t)node->seq_nmbr;

    if (packet_sequence_nmbr <= node_sequence_nmbr) {
        if (packet->destination != GLOBAL_BROADCAST) {
            // A broadcast replay is normal, but a unicast replay is odd. So
            // we only print out errors when it's a unicast replay
            ulog_error("ERROR: Packet Replayed. Packseq=%lu, Nodeseq=%lu",
                       packet_sequence_nmbr,
                       node_sequence_nmbr);
        }
        else {
            ulog("> Broadcast re-seen. Packseq=%lu, Nodeseq=%lu",
                 packet_sequence_nmbr, node_sequence_nmbr);
            return PacketSecurity_ReplayedBroadcast;
        }

        return PacketSecurity_Replayed;
    }

    // Set the counter to the last observed counter. This is also
    // for packets we are just passing through and not processed here
    node->seq_nmbr = packet->seq_nmbr;

    ulog("> Node %d Verification passed with seq %lu",
         node->node_id, packet_sequence_nmbr);
    return PacketSecurity_OK;
}

/**
 * @brief Returns the current sequence number stored here.
 *
 */
uint32_t replay_protection_get_current_sequence_number()
{
    return sequence_nmbr;
}

/**
 * @brief Sets the current sequence number stored here.
 *
 */
void replay_protection_set_current_sequence_number(uint32_t seq_nmbr)
{
    ulog("Changed sequence number to %lu", seq_nmbr);
    sequence_nmbr = seq_nmbr;
}

/**
 * Sets the replay protection flag on an incoming packet
 *
 * @param packet The secure packet to be secured
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_secure_packet(secure_packet_t *packet, uint16_t phy_src)
{
    if (sequence_nmbr >= REPLAY_OVERFLOW_HARD_LIMIT) {
        ulog_error("ERROR: Sequence number overflow");
        return PacketSecurity_Overflow;
    }

    sequence_nmbr++;

    if (packet->source == get_this_node_address()) {
        packet->seq_nmbr = sequence_nmbr;

        ulog("> Set packet sequence number to %lu", sequence_nmbr);
    }
    else {
        ulog_info("> Not incrementing packet seq cos it's not ours");
    }

    if (sequence_nmbr >= REPLAY_OVERFLOW_WARNING) {
        is_in_soft_overflow = 1;
        uint32_t leftover = sequence_nmbr - REPLAY_OVERFLOW_WARNING;

        if (leftover % 50 == 0) {
            // Send warning every 50 packets
            network_security_raise_event(NetworkSecurity_OverflowWarning, 0);
        }
    }

    return PacketSecurity_OK;
}

/**
 * @brief Sets the node overflow state. See header for extended info.
 */
void replay_protection_set_overflow(int val)
{
    is_in_soft_overflow = val;
}

/**
 * @brief Says if this node is currently in an overflow state. See header for
 *        extended info.
 */
int replay_protection_get_overflow()
{
    if (sequence_nmbr >= REPLAY_OVERFLOW_WARNING) {
        ulog("Node is in sequence number overflow");
        is_in_soft_overflow = 1;
    }

    return is_in_soft_overflow;
}

/**
 * @brief Resets this node sequence number
 */
void replay_protection_reset_sequence_number()
{
    sequence_nmbr = rand() % 30;
    ulog("> Reset node sequence number to %d", sequence_nmbr);
}

/**
 * Prints information about this component
 *
 */
void replay_protection_print_info()
{
    ulog("> Replay Seq: %lu", sequence_nmbr);
    ulog("> Replay Overflow: %d", is_in_soft_overflow);
}
