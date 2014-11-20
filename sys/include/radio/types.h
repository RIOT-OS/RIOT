/**
 * Generic radio driver interface data structures and prototypes
 *
 * Copyright (C) 2009  Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

/**
 * @file
 * @brief       Common network stack types (of all layers).
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @version     $Revision: 2061 $
 *
 * @note        $Id: common-types.h 2061 2010-04-01 12:13:22Z hillebra $
 */

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t protocol_t;         ///< Packet protocol type
typedef uint16_t radio_address_t;   ///< Radio layer address type

#define NUM_PRIORITY_LEVELS 3       ///< Number of packet transmission priorities

/**
 * @brief   Packet transmission priorities of various layers.
 */
enum transmission_priorities {
    PRIORITY_ALARM   = 0,
    PRIORITY_WARNING = 1,
    PRIORITY_DATA    = 2
};

/**
 * Additional packet information on a received packet.
 * This struct is passed along all receive functions of
 * all layers. Each layers fills in additional information.
 */
typedef struct __attribute__((packed)) packet_info_t {
    uint16_t source;            ///< Net layer: source
    uint16_t destination;       ///< Net layer: destination
    radio_address_t phy_src;    ///< Radio layer: source
    uint8_t rssi;               ///< Radio layer: RSSI
    uint8_t lqi;                ///< Radio layer: LQI
    uint8_t ttl;                ///< Net layer: TTL
    uint8_t tos;                ///< Net layer: TOS
    bool promiscuous;           ///< Radio layer: whether network interface is in promiscuous mode
} packet_info_t;


/**
 * @brief General link layer packet format
 */
typedef struct __attribute__((packed))
{
    uint8_t processing;     ///< internal processing state
    uint16_t src;           ///< Radio source address
    uint16_t dst;           ///< Radio destination address
    uint8_t rssi;           ///< Radio Signal Strength Indication
    uint8_t lqi;            ///< Link Quality Indicator
    timex_t toa;            ///< Time of Arrival
    radio_packet_length_t length;         ///< Length of payload
    uint8_t *data;          ///< Payload
}
radio_packet_t;


/**
 * Packet handler (receive function) of all layers.
 * @param [in/out]  payload         Pointer to packet payload data
 * @param [in]      payload_size    Size of the packet payload data in bytes
 * @param [in/out]  packet_info     Cross-layer meta data
 */
typedef void (*packet_handler_t)(void *payload, int payload_size, packet_info_t *packet_info);

/**
 * Packet monitor of all layers. Normally there can be one packet
 * monitor per layer (if any). The payload must not be changed!
 *
 * @param   payload         Pointer to packet payload data
 * @param   payload_size    Size of the packet payload data in bytes
 * @param   protocol        Protocol type of the packet payload data
 * @param   packet_info     Cross-layer meta data
 */
typedef void (*packet_monitor_t)(void *payload, int payload_size, protocol_t protocol, packet_info_t *packet_info);

#ifdef __cplusplus
}
#endif

#endif /* COMMONTYPES_H_ */
