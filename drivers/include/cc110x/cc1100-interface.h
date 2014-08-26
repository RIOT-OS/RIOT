/******************************************************************************
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
*******************************************************************************/

/**
 * @ingroup     dev_cc110x
 * @{
 */

/**
 * @file
 * @brief       TI Chipcon CC110x public interface
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 2283 $
 *
 * @note        $Id: cc1100-interface.h 2283 2010-06-15 14:02:27Z hillebra $
 */

#ifndef CC1100INTERFACE_H_
#define CC1100INTERFACE_H_

#include <stdint.h>
#include "radio/radio.h"

#define CC1100_BROADCAST_ADDRESS (0x00) ///< CC1100 broadcast address

#define MAX_UID                  (0xFF) ///< Maximum UID of a node is 255
#define MIN_UID                  (0x01) ///< Minimum UID of a node is 1

#define MIN_CHANNR                  (0) ///< Minimum channel number
#define MAX_CHANNR                 (24) ///< Maximum channel number

#define MIN_OUTPUT_POWER            (0) ///< Minimum output power value
#define MAX_OUTPUT_POWER           (11) ///< Maximum output power value

#define CC1100_MODE_WOR             (0) ///< Usable radio mode: Wake-On-Radio
#define CC1100_MODE_CONSTANT_RX     (1) ///< Usable radio mode: Constant receive

#define CC1100_MAX_DATA_LENGTH (58)

// Define default radio mode to constant RX if no
// application specific setting is available.
#ifndef CC1100_RADIO_MODE
#ifdef MODULE_RPL
#warning RPL currently works with CC1100_MODE_WOR
#endif
#define CC1100_RADIO_MODE CC1100_MODE_CONSTANT_RX
#endif

/// CC1100 radio interface
extern const radio_t radio_cc1100;

/**
 * @brief   Initialize radio layer.
 *
 * Initialize the radio layer, must be called before radio can be used.
 */
void cc1100_init(void);

/**
 * @brief   Get the radio mode.
 *
 * @return  Either CC1100_MODE_CONSTANT_RX or CC1100_MODE_WOR.
 */
uint8_t cc1100_get_mode(void);

/**
 * @brief   Set the radio mode.
 *
 * Sets the radio mode with optional mode data. If the radio mode is WOR,
 * the optional mode data is the RX interval in milliseconds. Must be called
 * only after CC1100 is initialized.
 *
 * @param mode New radio mode (one of the constants defined in this file).
 * @param opt_mode_data Desired RX interval in milliseconds [50..60000].
 *
 * @return true if mode could be applied; false otherwise (stays in previous mode).
 */
bool cc1100_set_mode(uint8_t mode, uint16_t opt_mode_data);

/**
 * @brief   Get the average transmission duration (till ACK received).
 *
 * @return  The average transmission duration of one packet in milliseconds.
 */
int cc1100_get_avg_transmission_duration(void);

/**
 * @brief   Get the radio address.
 *
 * @return  The current address of the radio.
 */
radio_address_t cc1100_get_address(void);

/**
 * @brief   Set the radio address.
 *
 * @param   address The new radio address.
 *
 * @return  true if address could be set; false otherwise.
 */
bool cc1100_set_address(radio_address_t address);

/**
 * @brief   Get the current channel number.
 *
 * @return  The current channel number used.
 */
uint8_t cc1100_get_channel(void);

/**
 * @brief   Set the channel to use.
 *
 * @param   channr The new channel number (between MIN_CHANNR and MAX_CHANNR) to use.
 *
 * @return  true if channel could be set; false otherwise.
 */
bool cc1100_set_channel(uint8_t channr);

/**
 * @brief   Set current output power in dBm.
 *
 * @param   pa_idx New output power setting, valid values
 *          are from MIN_OUTPUT_POWER (lowest output power, -52 dBm)
 *          to MAX_OUTPUT_POWER (highest output power, +10 dBm).
 *
 * @return  true if output power could be set; false otherwise.
 */
bool cc1100_set_output_power(uint8_t pa_idx);

/**
 * @brief   Set a packet monitor at this layer.
 *
 * All incoming packets will be passed to this packet monitor before
 * packet gets processed by a specific handler. The work in the monitor
 * should take as less time as possible (no waiting or intense I/O).
 *
 * @param   monitor The packet monitor or NULL if the current packet
 *                  monitor should be removed.
 *
 * @return  true if this layer supports packet monitoring; false otherwise.
 */
bool cc1100_set_packet_monitor(packet_monitor_t monitor);

/**
 * @brief   Set a packet handler for a given protocol.
 *
 * @param   protocol The protocol identifier (3-bit value > 0, 1 reserved for LL-ACK).
 * @param   handler The packet handler called if a packet of given protocol is received.
 *
 * @return  -1 if an error occurs (e.g. handler table full) else >= 0.
 */
int cc1100_set_packet_handler(protocol_t protocol, packet_handler_t handler);

/**
 * @brief   Send data to given destination address with acknowledgment.
 *
 * The maximum payload length are 58 bytes!
 *
 * @param   addr The destination address.
 * @param   protocol The protocol identifier of the transmitted data.
 * @param   priority Ignored (always highest).
 * @param   payload The data to be send.
 * @param   payload_len The length of the data in bytes.
 *
 * @return  A negative value if operation failed; if operation succeeded (ACK received) the number of transmitted bytes.
 */
int cc1100_send(radio_address_t addr, protocol_t protocol, int priority, char *payload, radio_packet_length_t payload_len);

/**
 * @brief   Send data to given destination address with acknowledgment and CSMA/CA.
 *
 * The maximum payload length are 58 bytes!
 *
 * @param   address The destination address.
 * @param   protocol The protocol identifier of the transmitted data.
 * @param   priority The MAC priority to send with. One of {PRIORITY_ALARM, PRIORITY_WARNING, PRIORITY_DATA}.
 * @param   payload The data to be send.
 * @param   payload_len The length of the data in bytes.
 *
 * @return  A negative value if operation failed; if operation succeeded (ACK received) else the number of transmitted bytes.
 */
int cc1100_send_csmaca(radio_address_t address, protocol_t protocol, int priority, char *payload, radio_packet_length_t payload_len);

/**
 * @brief   Print current radio configuration.
 */
void cc1100_print_config(void);

/**
 * @brief   Print radio statistics.
 */
void cc1100_print_statistic(void);

/** @} */

#endif /* CC1100INTERFACE_H_ */
