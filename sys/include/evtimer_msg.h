/*
 * SPDX-FileCopyrightText: 2016-2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup  sys_evtimer
 * @{
 *
 * @file
 * @brief       IPC-based evtimer definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "msg.h"
#include "evtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IPC-message event timer
 * @extends evtimer_t
 */
typedef evtimer_t evtimer_msg_t;

/**
 * @brief   IPC-message event
 * @extends evtimer_event_t
 */
typedef struct {
    evtimer_event_t event;      /**< base class */
    msg_t msg;                  /**< the IPC message to generate on event */
} evtimer_msg_event_t;

/**
 * @brief   Adds event to an event timer that handles events via IPC
 *
 * @param[in] evtimer       An event timer
 * @param[in] event         An event
 * @param[in] target_pid    The PID of the thread that should receive the IPC
 *                          message
 */
static inline void evtimer_add_msg(evtimer_msg_t *evtimer,
                                   evtimer_msg_event_t *event,
                                   kernel_pid_t target_pid)
{
    /* use sender_pid field to get target_pid into callback function */
    event->msg.sender_pid = target_pid;
    evtimer_add(evtimer, &event->event);
}

/**
 * @brief   Event handler for IPC messages
 *
 * @param[in] event     The event to handle
 */
static inline void _evtimer_msg_handler(evtimer_event_t *event)
{
    evtimer_msg_event_t *mevent = (evtimer_msg_event_t *)event;
    msg_send_int(&mevent->msg, mevent->msg.sender_pid);
}

/**
 * @brief   Initializes event timer to handle events via IPC
 *
 * @param[in] evtimer   An event timer
 */
static inline void evtimer_init_msg(evtimer_t *evtimer)
{
    evtimer_init(evtimer, _evtimer_msg_handler);
}

#ifdef __cplusplus
}
#endif

/** @} */
