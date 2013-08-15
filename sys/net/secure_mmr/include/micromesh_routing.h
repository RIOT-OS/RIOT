/*
 * micromesh_routing.h - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file  		micromesh_routing.h
 * @brief		Micro Mesh Routing
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date		15.09.2013 12:05:15
 */


#ifndef MICROMESH_H_
#define MICROMESH_H_

#define MMR_TYPE_RREQ		 	        (31)
#define MMR_TYPE_RREP		 	        (32)
#define MMR_TYPE_RERR		 	        (33)
#define RERR_NODE_UNREACHABLE	        (34)
#define CONSTANT_SECOND					(1)
#define RREQ_ID_SEQUENCE_NUMBER_START	(1)
// Change this to reduce re-tries
#define RREQ_THRESHOLD					(1)
// Send no RREQs for these messages, value
#define RREQ_NONE						(0xFF)
// must be greater than RREQ_THRESHOLD
#define TTL_START						(10)
#define TTL_THRESHOLD					(10)
#define RREQ_TIMEOUT_BASE		        (2*CONSTANT_SECOND)
#define RREQ_TIMEOUT_PER_TTL	        (1*CONSTANT_SECOND)
// Get the broadcast address of a net address
#define NETWORK_ADDR_BC(x)		        (x&0xff00)
// Read NET part of net address
#define NETWORK_ADDR_NET(x)		 	    (x>>8)
// Read HOST part of net address
#define NETWORK_ADDR_HOST(x)	        (x&0x00ff)
#define MESSAGE_QUEUE_SIZE	            (20)


/*---------------------------------------------------------------------------*/
//					Data structures
/*---------------------------------------------------------------------------*/
typedef struct packet_queue_entry_t {
    secure_packet_t packet;
    volatile uint32_t timestamp;
    uint8_t retry_count;
} packet_queue_entry_t;


typedef struct mmr_stat {
    uint32_t rreq_originated;
    uint32_t rrep_originated;
    uint32_t rerr_originated;
    uint32_t rreq_received;
    uint32_t rrep_received;
    uint32_t rerr_received;
    // RREQ found no route
    uint32_t messages_no_route_found;
    // Forwarding: no route in route table
    uint32_t messages_no_route_avail_on_forward;
    // Forwarding: broken link detected
    uint32_t messages_broken_link_on_forward;
    uint32_t rreq_duplicated;
} mmr_stat_t;

/*
 * @brief  Enumeration of the possible route errors
 * */
enum route_errors {
    ROUTE_ERROR_NONE				=  1,	// No route error
    ROUTE_ERROR_UNKNOWN				=  0,	// Unknown route error
    ROUTE_ERROR_MISSING_ROUTE		= -1,	// Missing route on forward
    ROUTE_ERROR_BROKEN_ROUTE		= -2	// Broken route on forward
};

enum Routing_Result {
    // No error
    Routing_OK							=  0,
    // Packets was processed by layer
    Routing_PacketHandledNoRebroadcast	=  1,
    // Packets was processed by layer and needs to be re-broadcast
    Routing_PacketHandledRebroadcast	=  2,
};

/*
 * @brief  The number of addresses that can be included in a route-reply
 * */
#define ADDRESS_LIST_SIZE		(17)

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
typedef struct __attribute__((packed)) mmr_rreq_message_t {
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
typedef struct __attribute__((packed)) mmr_rrep_message_t {
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
typedef struct __attribute__((packed)) mmr_rerr_message_t {
    uint8_t type;	///< Must be first byte in struct for type detection
    uint8_t error_type;
    uint16_t destination;
    uint16_t next_hop_address;
    uint16_t node_id;
} mmr_rerr_message_t;


/**
 * @brief	Initialize MMR layer.
 */
void micromesh_init(void);

/**
 * Called to request transmission of a packet that requires routing.
 * It is the responsibility of the routing layer to provide
 * a best-effort transmission of this packet to an appropriate next hop by
 * calling the networks layer sending routines once this routing information
 * becomes available.
 *
 * @param	packet outgoing packet
 *
 * @return 	true if packet was successfully stored for transmission;
 *          false otherwise (e.g. message queue full).
 */
bool micromesh_send(secure_packet_t *packet);

/**
 * Called by the network layer which forwards notifications of dropped packets
 * from the link layer.
 *
 * @param 	packet dropped network packet
 * @param	next_hop next hop network address of dropped packet
 *          (can be undefined)
 * @param	error Error type which informs about reason
 */
void micromesh_packet_dropped(secure_packet_t *message, uint16_t next_hop,
                              int error);

/**
 * @brief	Receive a message from network layer.
 *
 * @param	packet message received
 * @param	packet_info Additional packet information
 */
int micromesh_receive(secure_packet_t *packet, packet_info_t *packet_info);

/**
 * @brief	Called when a message that is passing through is to be examined
 *
 * @param	packet message received
 */
void micromesh_peek(secure_packet_t *packet);

/**
 * @brief	Print routing layer statistics.
 */
void micromesh_print_stats(void);

#endif /* MICROMESH_H_ */
