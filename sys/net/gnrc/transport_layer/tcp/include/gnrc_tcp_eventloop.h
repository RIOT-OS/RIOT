/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP event loop declarations.
 *
* @author       Simon Brummer <simon.brummer@posteo.de>
 */


#include <stdint.h>

#include "evtimer_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts GNRC TCP's main processing thread.
 *
 * @retval  PID of processing thread on success
 * @retval  -EEXIST if processing thread was already started
 * @retval  see @ref thread_create() for more error cases.
 */
int _gnrc_tcp_eventloop_init(void);

/**
 * @brief   Schedule event to event loop
 *
 * @param[in] event     The event to schedule
 * @param[in] offset    Offset in milliseconds when the event should be handled
 *                      in the event loop
 * @param[in] type      Type of the message for the event
 * @param[in] context   Context of the event.
 */
void _gnrc_tcp_eventloop_sched(evtimer_msg_event_t *event, uint32_t offset,
                               uint16_t type, void *context);

/**
 * @brief   Unschedule event to event loop
 *
 * Does nothing if @p event was not scheduled.
 *
 * @param[in] event The event to unschedule
 */
void _gnrc_tcp_eventloop_unsched(evtimer_msg_event_t *event);

#ifdef __cplusplus
}
#endif

/** @} */
