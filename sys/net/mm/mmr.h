/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
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

#ifndef MMR_H_
#define MMR_H_

/**
 * @file
 * @internal
 * @brief		Micro Mesh Routing
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: mmr.h 3854 2011-12-06 15:27:01Z hwill $
 */

#include "net-types.h"

#define MMR_TYPE_RREQ		 	 (1)
#define MMR_TYPE_RREP		 	 (2)
#define MMR_TYPE_RERR		 	 (3)

#define ADDRESS_LIST_SIZE		(21)

#define RERR_NODE_UNREACHABLE	 (1)

/**
 * Represents a Route Request (RREQ) message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |     Length    |           Destination         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |             Source            |          Address[1..n]        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct __attribute__((packed)) {
    uint8_t type;	///< Must be first byte in struct for type detection
    uint8_t length;
    uint16_t destination;
    uint16_t source;
    uint16_t address[ADDRESS_LIST_SIZE];
} mmr_rreq_message_t;

/**
 * Represents a Route Reply (RREP) message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |     Length    |           Destination         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |             Source            |          Address[1..n]        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct __attribute__((packed)) {
    uint8_t type;	///< Must be first byte in struct for type detection
    uint8_t length;
    uint16_t destination;
    uint16_t source;
    uint16_t address[ADDRESS_LIST_SIZE];
} mmr_rrep_message_t;

/**
 * Represents a Route Error (RERR) message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |   Error Type  |  Type-Specific Information    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Valid Error Types are:
 *
 * 1 = NODE_UNREACHABLE
 *
 * Node Unreachable Type-Specific Information:
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Unreachable Node Address    |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
typedef struct __attribute__((packed)) {
    uint8_t type;	///< Must be first byte in struct for type detection
    uint8_t error_type;
    uint16_t type_specific_info;
} mmr_rerr_message_t;

/**
 * @brief	Initialize MMR layer.
 */
void mmr_init(void);

/**
 * Called by the network layer for every incoming packet. A routing
 * implementation may wish to look at these packets for informational
 * purposes, but should not change their contents.
 *
 * @param	message incoming packet
 * @param	packet_info Additional packet information
 */
void mmr_peek(net_message_t *message, packet_info_t *packet_info);

/**
 * Called by the network layer to request transmission of a packet that
 * requires routing. It is the responsibility of the routing layer to provide
 * a best-effort transmission of this packet to an appropriate next hop by
 * calling the networks layer sending routines once this routing information
 * becomes available.
 *
 * @param	message outgoing packet
 *
 * @return 	true if packet was successfully stored for transmission; false otherwise
 *          (e.g. message queue full).
 */
bool mmr_send(net_message_t *message);

/**
 * Called by the network layer which forwards notifications of dropped packets
 * from the link layer. Not all MAC implementations support this feature!
 *
 * @param 	message dropped network packet
 * @param	next_hop next hop network address of dropped packet (can be undefined)
 * @param	error Error type which informs about reason
 */
void mmr_packet_dropped(net_message_t *message, uint16_t next_hop, int error);

/**
 * @brief	Receive a message from network layer.
 *
 * @param	msg message received
 * @param	msg_size Size of received message
 * @param	packet_info Additional packet information
 */
void mmr_receive(void *msg, int msg_size, packet_info_t *packet_info);

/**
 * @brief	Print routing layer statistics.
 */
void mmr_print_stats(void);

#endif /* MMR_H_ */
