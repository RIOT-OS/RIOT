/**
 * The PulseSync clock synchronization protocol
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup pulsesync    PulseSync (modified FTSP)
 * @ingroup  net
 * PulseSync is very similar to the flooding time synchronization protocol.
 *
 * The main difference is that nodes which receive synchronization beacons relay
 * the received information almost immediately regardless of their own
 * synchronization state. This change in behavior leads to a faster data
 * dissemination and therefore shorter synchronization rounds.
 * However, this protocol can be problematic in very dense network topologies
 * because of a high likelihood of collisions.
 * @see      <a href="http://dl.acm.org/citation.cfm?id=1644061">
 *              Lenzen et.al.: Optimal clock synchronization in networks
 *           </a>
 * @{
 * @file     pulsesync.h
 * @brief    Declarations for the PulseSync clock synchronization protocol.
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __PULSESYNC_H
#define __PULSESYNC_H

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
pulsesync_beacon_t;

/**
 * A single regression table entry.
 */
typedef struct table_item {
    uint8_t state;              /// << Indicates if the entries is used or not
    uint64_t local;             /// << Recorded local time
    uint64_t global;            /// << Received global time
} pulsesync_table_item_t;


/**
 * @brief Starts the PulseSync module
 */
void pulsesync_init(void);

/**
 * @brief sets the beacon interval in seconds.
 */
void pulsesync_set_beacon_delay(uint32_t delay_in_sec);

/**
 * Sets the minimal delay between sending and receiving a beacon.
 * To be more specific: Let t_s be the time when a timestamp is applied to a beacon
 * shortly before it is sent and t_r the time when the time of arrival
 * of this sent beacon is recorded at the receiver. The difference of t_s
 * and t_r then signifies the delay between the sending and receiving of a beacon.
 * This delay has to be determined for every platform (read: for different MCU /
 * transceiver combinations).
 */
void pulsesync_set_prop_time(uint32_t us);

/**
 * @brief Causes pulsesync to stop sending beacons / ignoring received beacons.
 */
void pulsesync_pause(void);

/**
 * @brief Causes pulsesync to restart/start sending beacons and processing received beacons.
 */
void pulsesync_resume(void);

/**
 * @brief reads a frame supplied by the mac layer of sixlowpan.
 * This function should only be called by mac.c
 */
void pulsesync_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa);

/**
 * @brief Refreshes the timestamp in a frame.
 * This function is executed shortly before transmitting a packet.
 * The function should only be executed by a transceiver driver.
 */
void pulsesync_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length);

/**
 * @brief Answers whether this node is synched against the root or not.
 */
int pulsesync_is_synced(void);

#endif /* __PULSESYNC_H */
