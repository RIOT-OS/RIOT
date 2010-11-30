/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

/**
 * @file
 * @brief		Common network stack types (of all layers).
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 2061 $
 *
 * @note		$Id: common-types.h 2061 2010-04-01 12:13:22Z hillebra $
 */

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t protocol_t;			///< Packet protocol type
typedef uint16_t radio_address_t;	///< Radio layer address type

#define NUM_PRIORITY_LEVELS	3		///< Number of packet transmission priorities

/**
 * @brief	Packet transmission priorities of various layers.
 */
enum transmission_priorities
{
	PRIORITY_ALARM   = 0,
	PRIORITY_WARNING = 1,
	PRIORITY_DATA    = 2
};

/**
 * Additional packet information on a received packet.
 * This struct is passed along all receive functions of
 * all layers. Each layers fills in additional information.
 */
typedef struct __attribute__ ((packed)) packet_info_t
{
	uint16_t source;			///< Net layer: source
	uint16_t destination;		///< Net layer: destination
	radio_address_t phy_src;	///< Radio layer: source
	uint8_t rssi;				///< Radio layer: RSSI
	uint8_t lqi;				///< Radio layer: LQI
	uint8_t ttl;				///< Net layer: TTL
	uint8_t tos;				///< Net layer: TOS
	bool promiscuous;			///< Radio layer: whether network interface is in promiscuous mode
} packet_info_t;


/**
 * @brief General link layer packet format
 */
typedef struct __attribute__ ((packed)) {
    uint8_t processing;     ///< internal processing state
    uint16_t src;           ///< Radio source address
    uint16_t dst;           ///< Radio destination address
    uint8_t rssi;           ///< Radio Signal Strength Indication
    uint8_t lqi;            ///< Link Quality Indicator
    uint8_t length;         ///< Length of payload
    uint8_t *data;          ///< Payload
} radio_packet_t;


/**
 * Packet handler (receive function) of all layers.
 * @param [in/out]	payload			Pointer to packet payload data
 * @param [in]		payload_size	Size of the packet payload data in bytes
 * @param [in/out]	packet_info		Cross-layer meta data
 */
typedef void (*packet_handler_t)(void* payload, int payload_size, packet_info_t* packet_info);

/**
 * Packet monitor of all layers. Normally there can be one packet
 * monitor per layer (if any). The payload must not be changed!
 *
 * @param	payload			Pointer to packet payload data
 * @param	payload_size	Size of the packet payload data in bytes
 * @param	protocol		Protocol type of the packet payload data
 * @param	packet_info		Cross-layer meta data
 */
typedef void (*packet_monitor_t)(void* payload, int payload_size, protocol_t protocol, packet_info_t* packet_info);

#endif /* COMMONTYPES_H_ */
