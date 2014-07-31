/**
 * pulsesync.h - Declarations and types for the Flooding Time Synchronization Protocol.
 *
 * Copyright (C) 2014  Philipp Rosenkranz
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup pulsesync    PULSESYNC - Gradient Time Synchronisation Protocol.
 * @ingroup  net
 * @brief    The Flooding Time Synchronization Protocol is clock synchronization protocol
 *           which tries to synchronizes not only the clock values but also the clock rate
 *           of all nodes in a network. It builds a spanning tree and synchronizes every node
 *           to their root. The level of a node in that tree is determined by its ID relative to
 *           its neighboring node.
 * @see      <a href="http://dl.acm.org/citation.cfm?id=1031501">
 *              Maroti et.al.: The flooding time synchronization protocol
 *           </a>
 * @{
 * @file     pulsesync.h
 * @brief    Declarations for the Flooding Time Synchronization Protocol.
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __PULSESYNC_H
#define __PULSESYNC_H

#include "gtimer.h"
#include "radio/types.h"

typedef struct  __attribute__((packed)) {
	uint8_t dispatch_marker; // << protocol marker
    uint16_t id;
    uint16_t root;
    uint16_t seq_number;
    uint64_t local;
    int64_t offset;
    float relative_rate; // << sender logical clockrate
} pulsesync_beacon_t;


typedef struct pulsesync_sync_point {
    uint16_t src; // TODO: only for debugging
    uint64_t local;
    int64_t offset;
} pulsesync_sync_point_t;

enum
{
  PULSESYNC_OK = 1,
  PULSESYNC_ERR = 0,
};

enum
{
    PULSESYNC_MAX_ENTRIES           = 8,              // number of entries in the table
    PULSESYNC_ROOT_TIMEOUT          = 3,              // time to declare itself the root if no msg was received (in sync periods)
    PULSESYNC_IGNORE_ROOT_MSG       = 4,              // after becoming the root ignore other roots messages (in send period)
    PULSESYNC_ENTRY_VALID_LIMIT     = 4,              // number of entries to become synchronized
    PULSESYNC_ENTRY_SEND_LIMIT      = 3,              // number of entries to send sync messages
    PULSESYNC_ENTRY_THROWOUT_LIMIT  = 300,             // if time sync error is bigger than this (in 32 kHz ticks) clear the table
};

enum
{
    PULSESYNC_ENTRY_EMPTY = 0,
    PULSESYNC_ENTRY_FULL  = 1,
};

typedef struct table_item
{
    uint8_t             state;
    uint64_t local_time;
    int64_t                time_offset;        // global-time - local_time
} table_item;




/**
 * @brief Starts the PULSESYNC module
 */
void pulsesync_init(void);

/**
 * @brief sets the beacon interval in seconds.
 */
void pulsesync_set_beacon_delay(uint32_t delay_in_sec);

/**
 * @brief sets the minimal delay between sending and receiving a beacon.
 * To be more specific: Let t_s be the time when a timestamp is applied to a beacon
 * shortly before it is sent and t_r the time when the time of arrival
 * of this sent beacon is recorded at the receiver. The difference of t_s
 * and t_r then signifies the delay between sending and receiving a beacon.
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
