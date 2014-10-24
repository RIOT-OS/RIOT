/*
 * Clock-sync protocol evaluation module.
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup clocksync_eval
 * @ingroup  net
 * A helper module which can be used to measure the performance of clock-sync
 * protocols.
 *
 * @{
 * @file     clocksync_eval.h
 * @brief    Declarations for the clock-sync. evaluation module.
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */
#ifndef __CLOCKSYNC_EVAL_H_
#define __CLOCKSYNC_EVAL_H_

#include <stdint.h>

#include "gtimer.h"

/**
 * @brief Clocksync evaluation beacon / message format
 */
typedef struct  __attribute__((packed))
{
    uint8_t dispatch_marker;    /// << protocol marker
    uint32_t counter;           /// << sender beacon counter
} clocksync_eval_beacon_t;

/**
 * @brief Starts the clocksync_eval module
 */
void clocksync_eval_init(void);

/**
 * @brief reads a IEEE802.15.4 frame.
 * This function should only be called by receive_helper.c
 *
 * @param[in] payload pointer to the frame payload
 * @param src src transceiver source address
 * @param[in] gtimer_toa gtimer time of arrival timestamp
 */
void clocksync_eval_mac_read(uint8_t *payload, uint16_t src, gtimer_timeval_t *gtimer_toa);

/**
 * @brief sets the beacon interval in seconds.
 *
 * @param lower_delay_in_ms lower bound of the interval
 * @param jitter_in_ms upper bound of the interval
 */
void clocksync_eval_set_beacon_interval(uint32_t lower_delay_in_ms, uint32_t jitter_in_ms);

/**
 * @brief sets the frequency for printing heartbeat messages to the uart.
 */
void clocksync_eval_set_heartbeat_interval(uint32_t delay_in_ms);

/**
 * @brief causes clocksync_eval to stop sending beacons
 */
void clocksync_eval_pause_sending(void);

/**
 * @brief resumes beacon sending
 */
void clocksync_eval_resume_sending(void);

/**
 * @brief causes clocksync_eval to stop printing heartbeat messages to the uart.
 */
void clocksync_eval_pause_heartbeat(void);

/**
 * @brief resumes heartbeat message printing to the uart.
 */
void clocksync_eval_resume_heartbeat(void);

#endif /* __CLOCKSYNC_EVAL_H_ */
