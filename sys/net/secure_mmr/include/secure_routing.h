/*
 * secure_routing.h - Controller for the secure routing functionality.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        secure_routing.h
 *
 * @brief       Main controller for Secure Routing functionality.
 *              The secure routing class functions as the main controller
 *              class for the entire secure routing layer. It receives send
 *              requests from applications on the higher classes, secures the
 *              packets and passes them on to the radio interface.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 14:46:31
 */

#ifndef __SECURE_ROUTING_H
#define __SECURE_ROUTING_H

#include "secure_routing_globals.h"
#include "types.h"

/**
 * @brief the possible errors that can be returned by the secure-routing class
 */
enum SecureRouting_Result {
    SecureRouting_OK          =  0,    // No error
    SecureRouting_Error       = -1,    // Error
    SecureRouting_TooLong     = -2,    // Packet exceeds length
    SecureRouting_TooShort    = -3,    // Packet is not long enough
    SecureRouting_NotInited   = -4     // Node was not initialized
};

/**
 * Initializes the Secure Routing layer. The command is also passed on to
 * all sub components that may require initialization
 *
 * @return Whether initialization was successful as a SecureRouting_Result
 */
int secure_routing_init(void);

/**
 * Used to specify a particular protocol, and the handler that should be called
 * when packets using that protocol arrive
 *
 * @param protocol The protocol for which the handler should be set
 * @param handler  The pointer to the handler
 *
 * @return Either OK or Error as a SecureRouting_Result
 */
int secure_routing_sethandler(protocol_t protocol, packet_handler_t handler);

/**
 * Sends a packet through the secure function to any other node in the network
 * The packets are encrypted and routed to their destination. If a
 * route-request needs to be built up, the packets are not immediately sent out
 *
 * @param address     The radio address of the destination node
 * @param protocol    The protocol with which the packet should be sent
 * @param priority    The radio priority of the packet
 * @param ttl         The time-to-live for the packet
 * @param payoad      A pointer to the data payload to be sent
 * @param payload_len The length of the payload. Has a max length of
 *                    MAX_PAYLOAD_LEN
 *
 * @return Error code indicating if successful or not as SecureRouting_Result
 */
int secure_send(radio_address_t address, protocol_t protocol,
                int priority, int ttl, char *payload, int payload_len);

/**
 * @brief       Takes a byte buffer and puts it in the buffer for send via
 *              the radio
 *
 * @param       gateway    The node over which the packet should be sent to
 *                         reach its destination
 * @param       protocol   The protocol to send it with over the next layer
 * @param       priority   The priority of the packet
 * @param       packet     The pointer to the byte buffer
 * @param       packet_len The length of the byte buffer
 * @param       is_secure_packet Indicates if this is a secure packet or not.
 *              If not, will just be sent over radio
 * @param       send_attempts Number of times to try
 * @param       send_rerr_if_failed If the radio send fails, send a route
 *              error message
 *
 * @return If the send was successful or not
 */
int enqueue_for_send(radio_address_t gateway, uint8_t protocol,
                     uint8_t priority, uint8_t *packet, uint16_t packet_len,
                     int is_secure_packet, int send_attempts,
                     int send_rerr_if_failed);

/**
 * Function to executes particular internal functions of the secure routing
 * layer. Used for testing and analysis of the component
 *
 * @param cmd  The command to be executed
 *
 * @return Error code indicating the result of the command. Type dependent on
 *         the command executed
 */
int secure_routing_exec(char *cmd);

/**
 * @brief   Set the security level
 *
 * @param   sec_level New security level. One of the SecurityPreset enums
 */
void set_security_level_preset(uint8_t sec_level);



/**
 * @brief   Takes a byte buffer and sends it directly via the radio interface
 *          without further manipulation. Do not use if you have no reason to
 *          use it. Preferred function is enqueue_for_send
 */
int radio_send_private(radio_address_t gateway, uint8_t protocol,
                       uint8_t priority, uint8_t *packet, uint16_t packet_len,
                       int is_secure_packet, int send_attempts,
                       int send_rerr_if_failed);

#endif
