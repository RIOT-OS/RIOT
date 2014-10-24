/*
 * The gradient time synchronization protocol.
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup gtsp    GTSP (Gradient Time Synchronisation Protocol)
 * @ingroup  net
 * The Gradient Clock Synchronization Protocol is a decentralized clock
 * synchronization protocol which tries to synchronize not only the
 * clock values but also the clock rate of all nodes in a network.
 * @see      <a href="http://dl.acm.org/citation.cfm?id=1602171">
 *              Sommer et.al.: Gradient Clock Synchronization in Wireless Sensor Networks
 *           </a>
 * @{
 * @file     gtsp.h
 * @brief    Declarations for the Gradient Clock Synchronization Protocol.
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __GTSP_H
#define __GTSP_H

#include "gtimer.h"
#include "radio/types.h"

/**
 * @brief    Package send by GTSP
 */
typedef struct  __attribute__((packed))
{
    uint8_t dispatch_marker; /** << protocol marker */
    uint64_t local; /** << sender hardware time */
    uint64_t global; /** << sender logical time */
    float relative_rate; /** << sender logical clockrate */
}
gtsp_beacon_t;

/**
 * Stores received data from other nodes with associated local data
 */
typedef struct {
    /** Current local hardware time when beacon was processed */
    uint64_t local_local;
    /** Current local global time when beacon was processed */
    uint64_t local_global;
    /** Local current clock rate correction factor */
    float relative_rate;
    /** ID of the sending node */
    uint16_t src;
    /** Sender local time when message was sent */
    uint64_t remote_local;
    /** Sender global time when message was sent */
    uint64_t remote_global;
    /** Sender current clock rate correction factor */
    float remote_rate;
} gtsp_sync_point_t;

/**
 * @brief Starts the GTSP module
 */
void gtsp_init(void);

/**
 * @brief sets the beacon interval in seconds.
 */
void gtsp_set_beacon_delay(uint32_t delay_in_sec);

/**
 * @brief sets the minimal delay between sending and receiving a beacon.
 * To be more specific: Let t_s be the time when a timestamp is applied to a
 * beacon shortly before it is sent and t_r the time when the time of arrival
 * of this sent beacon is recorded at the receiver. The difference of t_s
 * and t_r then signifies the delay between sending and receiving a beacon.
 * This delay has to be determined for every platform (read: for different MCU /
 * transceiver combinations).
 */
void gtsp_set_prop_time(uint32_t us);

/**
 * @brief Causes gtsp to stop sending beacons / ignoring received beacons.
 */
void gtsp_pause(void);

/**
 * @brief Causes gtsp to restart/start sending beacons and processing received
 * beacons.
 */
void gtsp_resume(void);

/**
 * @brief reads a IEEE802.15.4 frame.
 * This function should only be called by receive_helper.c
 *
 * @param[in] payload pointer to the frame payload
 * @param src src transceiver source address
 * @param[in] gtimer_toa gtimer time of arrival timestamp
 */
void gtsp_mac_read(uint8_t *frame_payload, uint16_t src, gtimer_timeval_t *toa);

/**
 * @brief Refreshes the timestamp in a ieee802.15.4 frame.
 * This function is executed shortly before transmitting a packet.
 * The function should only be executed by a transceiver driver.
 */
void gtsp_driver_timestamp(uint8_t *ieee802154_frame, uint8_t frame_length);

#endif /* __GTSP_H */
