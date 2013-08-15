/*
 * packet_mac.h - MAC for single packets.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        packet_mac.h
 * @brief       Used to mac single packets
 *              Used to mac single packets
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger   <nicolai.schmittberger@fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 12:44:45
 */

#ifndef __PACKET_MAC_H
#define __PACKET_MAC_H


#define AES

/**
 * Returns the message-authentication component
 *
 * @return the message-authentication component
 */
packet_security_component_t *packet_mac_get_component(void);

/**
 * Initializes the message-authentication component
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int packet_mac_init_component(void);

/**
 * Writes a message-authentication code to the last 4
 * bytes of the packet
 *
 * @param The packet that should be secured
 * @param phy_src The physical source to be used for securing
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int packet_mac_secure_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * Verifies that the message-authentication code is valid for
 * the specified packet
 *
 * @param packet The packet that should be verified
 * @param phy_src The physical source the packet came on
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int packet_mac_verify_packet(secure_packet_t *packet, uint16_t phy_src);

/**
 * Gets the number of wrong macs that have been recorded by
 * the component (over a pre-defined period). This is not then
 * current wrong mac count, but over the last completed time
 * period (e.g 10 minutes) since this component was called
 *
 * @param was_reset  indicates if the last 10 mins recorded has been reset
 *                   already
 *
 * @return Number of failed message-authentications
 */
int packet_mac_get_wrong_mac_count(int *was_reset);

/**
 * Sets the number of wrong macs that have been recorded by
 * the component
 *
 */
void packet_mac_set_wrong_mac_count(int new_wrong_macs);

/**
 * Resets the recorded message-authentication errors back
 * to 0. Would usually be done after reading the value out.
 * This resets the value for last 10 minutes, and not the
 * currently running mac counter
 *
 */
void packet_mac_reset_mac_errors(void);


/**
 * @brief Forces the component to have a mac error every 2nd packet
 *        for debugging
 */
void packet_mac_set_force_mac_errors(int val);


/**
 * @brief Disables or enables this component
 *
 * @param val Bool indicating if to enable or disable
 *
 */
void packet_mac_enable(int val);

#endif
