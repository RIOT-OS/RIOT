/*
 * secure_routing_globals.h - Definitions for a the secure routing.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

#ifndef __SECURE_HEADER_H
#define __SECURE_HEADER_H

/*
 * @file        secure_routing_globals.h
 * @brief       Contains all the global variables and defines for the secure
 *              routing
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 13:43:31
 */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mutex.h>
#include "secure_routing_config.h"


/**
 * @brief The possible errors that can be returned by the secure-routing
 *        class. Maximum of 3 bits allowed!
 */
enum SecurityMode {
    // No security at all on the packet
    SecurityMode_None = 0,
    // The packet has message-authentication only
    SecurityMode_MAC = 1,
    // MAC packet with initial key. Used for HELLO and ACK
    SecurityMode_MACWithInitialKey = 2,
    // The packet is encrypted, but without message-auth
    SecurityMode_Encrypt = 3,
    // The packet is encrypted, message-auth, with global key
    SecurityMode_EncryptAndMAC = 4,
    // Special mode that does not use the current key, but previous key.
    // For Global Key-Exchange
    SecurityMode_EncryptAndMACPriorKey = 5,
    // The packet is encrypted, message-auth, with pairwise key
    SecurityMode_EncryptAndMACPairwise = 6,
    // The packet is encrypted, message-auth, with pairwise key, and is to
    // broadcasted
    SecurityMode_EncryptAndMACPairwiseBroadcast = 7
    // Do not add anymore modes, as this will use 4 bits. Max is 7 = 111
};

/**
 * @brief The possible errors that can be returned by the secure-routing
 *        class. Maximum of 3 bits allowed!
 */
enum SecurityPreset {
    // Disable security
    SecurityPresetNone          = 0,
    // Only authentication, avoid pairwise key, key-refresh interval
    // is long
    SecurityPresetLow           = 1,
    // Automatically manage and adapt security
    SecurityPresetOptimal       = 2,
    // Authenticate + Encrypt, always use pairwise keys, frequent keyrefresh
    SecurityPresetExtreme       = 3,
    // User defined values
    SecurityPresetUserDef       = 4
};


/**
 * @brief The possible errors that can be returned by the secure-routing class
 */
enum PeekHint {
    // 1000b Says packet should be peeked at by routing nodes
    PeekHint_Peek = 8,
    // 10000b No need to peek at packet, content only good for target
    PeekHint_DontPeek = 16
};

/**
 * @brief Global System Defines. These are not configuration options
 */
#define GLOBAL_BROADCAST   0       // Address that represents global broadcasts
#define LOCAL_BROADCAST    0       // Address that represents local broadcasts
#define MSG_EXIT           0x0001  // Request to exit when sent to threads

/**
 * @brief Option Defines. Change these to compile the params differently
 */
// #define _PACKET_PHY_SRC  // Enables the inclusion of physical source in
// packet header
// #define _24_BIT_SEQ      // Enables the change of bytes from 16 to 24 bits


/**
 * @brief Structure for the sequence number
 */

/*
#ifdef _24_BIT_SEQ
typedef struct sequence_number
{
#ifdef _24_BIT_SEQ
    unsigned val : 24;
#else
    unsigned val : 16;
#endif
} __attribute__((packed)) sequence_number_t ;
#else
#define sequence_number_t uint16_t
#endif
*/
#define sequence_number_t uint16_t

/**
 * @brief Security Packet structure. This is pre-pended to data being moved through
 *                the network
 *
 *  0               1               2               3                           4
 *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     TTL        |   Protocol    |  Destination  |     Source    |  Payload Len |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |           Sequence ID          |     Flags     | Seq Clear ID                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                            Payload (Variable Length)                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                    MAC(4)                     |              MAC(4)           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
// The physical source can be enabled by defining this. Enabling it will make
// the physical source be embedded into every packet. Removing it implies that
// packets will be verified via init vector physical source

#ifdef _PACKET_PHY_SRC
uint8_t  phy_source;  // The physical source ofthe packet
#endif

typedef struct secure_packet {
    uint8_t  ttl;                    // Time-to-live for this packet
    uint8_t  source;                 // The global source ofthe packet
    uint8_t  destination;            // The global destination of the packet
    uint8_t  protocol;               // The protocol of the payload of this
                                     // packet
    uint8_t  payload_len;            // The length of the data payload part of
                                     // this packet
    uint8_t  flags;                  // Flags Random (4 bits) | Peek (1 Bit) |
                                     // Security Mode (3 bits)
    uint16_t seq_nmbr;               // Sequence number (counter) of the packet
    uint8_t  data[MAX_PAYLOAD_LEN];  // The (encrypted) payload
    uint8_t  mac[SECURE_MAC_LEN];    // The message-authentication code
} __attribute__((packed)) secure_packet_t;


/**
 * @brief Global variables that are required in various locations
 */

/* The key is defined in the secure_routing.c file */
// The global encryption key used for packets
extern uint8_t global_key[SECURE_ROUTING_KEYSIZE];
// The prior global key used just before this one
extern uint8_t last_global_key[SECURE_ROUTING_KEYSIZE];
// The global encryption key used for packets
extern uint8_t initial_key[SECURE_ROUTING_KEYSIZE];
// The address of the basestation
extern uint8_t basestation_addr;
// Indicates if this node is the base station
extern uint8_t i_am_basestation;
// The current security level pre-set
extern uint8_t security_preset;
// Allows only one thread to process at a time
extern mutex_t  processing_mutex;

/*
 * Helper macros that need to be global because they are used in
 * various locations
 */

/**
 * @brief Returns the address of this node
 */
#define get_this_node_address     cc1100_get_address

/**
 * @brief Says if a particular secure packet is encrypted or if it's
 *        unencrypted
 */
#define IS_PACKET_UNENCRYPTED(x)  (x->flags == SecurityMode_None || \
                                   x->flags == SecurityMode_MAC)

/**
 * @brief Used to see if a command starts with a certain character string
 */
#define STARTS_WITH(cmd, txt) strncmp(cmd, txt, strlen(txt)) == 0

/**
 * @brief Use this macro to decide if an address is the same as the address of
 *        this node.
 *        This function can be easily expanded in future to include 16 bit
 *        addresses
 */
#define IS_LOCAL_ADDRESS(x) ((uint8_t)x == (uint8_t)get_this_node_address())

/**
 * @brief Use this macro to make the flags
 */
#define SECURE_FLAG(peek, security) ((uint8_t)peek | (uint8_t)security)

/**
 * @brief Use this macro to mask out all bits apart from the bottom 3 =
 *        security mode
 */
#define GET_SECURITY_MODE(flag) (flag & 7)

/**
 * @brief Use this macro to see if a packet we would route should be examined
 *        locally Move by 3, so the 4th bit is in lowest position, then mask
 *        out all apart from the lowest bit
 */
#define SHOULD_PEEK(flag) ((flag >> 3) & 1)

/**
 * @brief Use this macro to set the physical source of a packet on the pack
 */
#ifdef _PACKET_PHY_SRC
#define set_physical_source(packet) packet->phy_source = \
                                                        get_this_node_address()
#define set_physical_source_p(packet) packet.phy_source = \
                                                        get_this_node_address()
#else
#define set_physical_source(packet)
#define set_physical_source_p(packet)
#endif

/**
 * Converts 3 bytes to a 24 bit number but in a 32 bit container. A = LSBs,
 * C = MSBs
 *
 * c (000000001) B (00000010) A (00000100) =  000000001 00000010 00000100 =
 * 66052 decimal
 *
 *
 * @return the number
 */
static inline uint32_t three_bytes_to_24bits(uint8_t sequence_nmbrC,
		                                     uint8_t sequence_nmbrB,
		                                     uint8_t sequence_nmbrA)
{
    uint32_t val = (uint32_t)(sequence_nmbrC << 16) |
    		       (uint32_t)(sequence_nmbrB << 8)  |
    		       (uint32_t)sequence_nmbrA ;
    return val;
}

#endif
