/*
 * packet_security.h - Encryption of particular packets.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        packet_security.h
 * @brief       The packet security classes are used to encrypt particular
 *              packets
 *
 *
 * @author     Freie Universität Berlin, Computer Systems & Telematics
 * @author     Mark Essien <markessien@gmail.com>
 * @author     Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date       16.09.2013 13:06:35
 */

#ifndef __PACKET_SECURITY_H
#define __PACKET_SECURITY_H

#include "secure_routing.h"


/*---------------------------------------------------------------------------*/
//                                      Defines
/*---------------------------------------------------------------------------*/
// Max Number of packet security components available
#define PACKET_SECURITY_MAX_COMPONENT_COUNT 3


/**
 * @brief the possible errors that can be returned by the packet-security class
 */
enum PacketSecurity_Result {
    PacketSecurity_OK                 =  0,   // No error
    PacketSecurity_NoComponents       =  1,   // No components available.
                                              // Not an error
    PacketSecurity_Disabled           =  2,   // Packet security is disabled on
                                              // packets
    PacketSecurity_Error              = -1,   // Error
    PacketSecurity_EncryptionError    = -30,  // When the encryption could not
                                              // be done
    PacketSecurity_NoCiphers          = -31,  // No ciphers available. Probably
                                              // not initialized
    PacketSecurity_NotAvailable       = -10,  // Something is not availabled
    PacketSecurity_Replayed           = -11,  // A packet was replayed.
    PacketSecurity_ReplayedBroadcast  = -12,
    PacketSecurity_Overflow           = -13
};

/**
 * Interface for the submodules that do the actual security on the packets
 *
 */
typedef struct packet_security_component {
    char        *name;
    int (*init_component)(void);
    int (*secure_packet)(secure_packet_t *packet, uint16_t phy_src);
    int (*verify_packet)(secure_packet_t *packet, uint16_t phy_src);
    int (*desecure_packet)(secure_packet_t *packet, uint16_t phy_src);
} packet_security_component_t;


/**
 * @brief  Initializes the Packet Security
 *
 * @return Whether initialization was successful.
 */
int packet_security_init(void);

/**
 * @brief  Adds a sub-component to the security component
 *
 * @param  component The interface of the component to be added
 *
 * @return PacketSecurity_Result indicating if the component was added
 */
int packet_security_add_component(packet_security_component_t *component);

/**
 * @brief  Secures a packet by passing it to all sub-components to secure the
 *         packet
 *
 * @param  packet  The secure packet to be secured
 * @param  phy_src The radio physical source packet arrived on
 *
 * @return PacketSecurity_Result with the success or failure of the operation
 */
int secure_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * @brief  Examines a previously secured packet to see if it still secure.
 *         Primarily via the message-authentication code
 *
 * @param  packet  The secure packet to be verified
 * @param  phy_src The radio physical source packet arrived on
 *
 * @return PacketSecurity_Result with the success or failure of the operation
 */
int verify_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * @brief Takes a previously secured packet and desecures it (in-place).
 *        This would primarily be done by decrypting it.
 *
 * @param  packet  The secure packet to be desecured
 * @param  phy_src The radio physical source packet arrived on
 *
 * @return PacketSecurity_Result with the success or failure of the operation
 */
int desecure_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * @brief Switches on and off the packet security. If off, then packets won't
 *        be modified or verified at all
 *
 * @param  val True or False
 *
 */
void enable_packet_security(int val);

#endif
