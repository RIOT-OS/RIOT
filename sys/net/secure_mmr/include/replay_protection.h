/*
 * replayprotection.h - Protection against replayed packets.
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
 * @date        16.09.2013 13:11:31
 */

#ifndef __REPLAY_PROTECTION_H
#define __REPLAY_PROTECTION_H

/**
 * Returns the replay component
 *
 * @return the replay component
 */
packet_security_component_t *replay_protection_get_component(void);

/**
 * Initialises the replay component
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_init_component(void);

/**
 * Examines a packet and indicates if it passes the verification rules
 *
 * @param packet   The secure packet to be verified
 * @param phy_src  The physical source of the packet
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_verify_packet(secure_packet_t *packet, uint16_t phy_src);


/**
 * Sets the replay protection flag on an incoming packet
 *
 * @param packet   The secure packet to be secured
 * @param phy_src  The physical source of the packet
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int replay_protection_secure_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * Set the overflow flag
 */
void replay_protection_set_overflow(int val);

/**
 * Says if this node is currently in an overflow state. In that case
 * the node will stop processing packets till it receives a key-refresh
 * packet
 *
 * @return 1 if overflowed, 0 otherwise
 */
int replay_protection_get_overflow(void);

/**
 * Prints information about this component
 *
 */
void replay_protection_print_info(void);

/**
 * Returns the current sequence number stored here.
 *
 * @return Full sequence number as 24 bit number
 */
uint32_t replay_protection_get_current_sequence_number(void);


/**
 * @brief Sets the current sequence number stored here.
 *
 * @param seq_nmbr Set the new sequence number
 *
 */
void replay_protection_set_current_sequence_number(uint32_t seq_nmbr);

/**
 * @brief Resets the node sequence number to a new intial value
 *
 */
void replay_protection_reset_sequence_number(void);

#endif
