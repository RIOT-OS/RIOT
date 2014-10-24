/*
 * The flooding time synchronization protocol
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup ftsp FTSP (Flooding Time Synchronization Protocol)
 * @ingroup  net
 * The Flooding Time Synchronization Protocol is a clock synchronization protocol
 * which tries to synchronizes not only the clock values but also the clock rate
 * of all nodes in a network.
 *
 * The protocol builds an implicit spanning tree and synchronizes every node
 * to their root. Each node forwards synchronization information to their
 * children as soon as the node decides it is sufficiently synchronized
 * against its parent node.
 * @see      <a href="http://dl.acm.org/citation.cfm?id=1031501">
 *              Maroti et.al.: The flooding time synchronization protocol
 *           </a>
 * @{
 * @file     ftsp.h
 * @brief    Declarations for the Flooding Time Synchronization Protocol.
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __FTSP_H
#define __FTSP_H

#include "gtimer.h"
#include "radio/types.h"

/**
 * @brief FTSP beacon / message format
 */
typedef struct  __attribute__((packed))
{
    uint8_t dispatch_marker;    /// << protocol marker
    uint16_t id;                /// << ID of the sending node
    uint16_t root;              /// << ID of the root node
    uint16_t seq_number;        /// << ID of the beacon
    uint64_t global;            /// << Time of the root estimated by the sender
}
ftsp_beacon_t;

/**
 * A single regression table entry.
 */
typedef struct table_item {
    uint8_t state;              /// << Indicates if the entries is used or not
    uint64_t local;             /// << Recorded local time
    uint64_t global;            /// << Received global time
} ftsp_table_item_t;


/**
 * @brief Starts the FTSP module
 */
void ftsp_init(void);

/**
 * @brief sets the beacon interval in seconds.
 */
void ftsp_set_beacon_delay(uint32_t delay_in_sec);

/**
 * Sets the minimal delay between sending and receiving a beacon.
 * To be more specific: Let t_s be the time when a timestamp is applied to a beacon
 * shortly before it is sent and t_r the time when the time of arrival
 * of this sent beacon is recorded at the receiver. The difference of t_s
 * and t_r then signifies the delay between the sending and receiving of a beacon.
 * This delay has to be determined for every platform (read: for different MCU /
 * transceiver combinations).
 */
void ftsp_set_prop_time(uint32_t us);

/**
 * @brief Causes ftsp to stop sending beacons / ignoring received beacons.
 */
void ftsp_pause(void);

/**
 * @brief Causes ftsp to restart/start sending beacons and processing received beacons.
 */
void ftsp_resume(void);

/**
 * @brief reads a IEEE802.15.4 frame.
 * This function should only be called by receive_helper.c
 *
 * @param[in] payload pointer to the frame payload
 * @param src src transceiver source address
 * @param[in] gtimer_toa gtimer time of arrival timestamp
 */
void ftsp_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa);

/**
 * @brief Refreshes the timestamp in a frame.
 * This function is executed shortly before transmitting a packet.
 * The function should only be executed by a transceiver driver.
 */
void ftsp_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length);

/**
 * @brief Answers whether this node is synched against the root or not.
 */
int ftsp_is_synced(void);

#endif /* __FTSP_H */
