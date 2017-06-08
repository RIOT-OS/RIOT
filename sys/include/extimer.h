/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_extimer Timer for time-based events
 * @ingroup     sys_xtimer
 * @brief       Allows for multiple IPC-based events to be handled with one
 *              @ref xtimer_t
 *
 * This timer **is not** to be used for time-*critical* applications.
 * However, for applications with tolerances from multiple 10 microseconds this
 * timer is suitable.
 *
 * @{
 *
 * @file
 * @brief   extimer definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef EXTIMER_H_
#define EXTIMER_H_

#include <stdint.h>

#include "kernel_types.h"
#include "list.h"
#include "msg.h"
#include "mutex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type to represent an event
 */
typedef struct extimer_event {
    struct extimer_event *next; /**< next event for this timer */
    msg_t msg;                  /**< message to be used for the event */
    uint64_t time;              /**< absolute time in microseconds */
} extimer_event_t;

/**
 * @brief   Event timer
 */
typedef struct {
    extimer_event_t *events;    /**< future events */
    mutex_t mutex;              /**< mutex */
    xtimer_t timer;             /**< the timer */
} extimer_t;

/**
 * @brief   Initilizes event timer
 *
 * @param[in] timer A timer.
 */
static inline void extimer_init(extimer_t *timer)
{
    timer->events = NULL;
    mutex_init(&timer->mutex);
    timer->timer.target = 0;
    timer->timer.long_target = 0;
}

/**
 * @brief   Get's absolute time in @p offset microseconds
 *
 * @see @ref xtimer_now64()
 *
 * In case of an integer overflow the function returns UINT64_MAX.
 *
 * @param[in] offset    The offset from the current absolute time.
 *
 * @return  The time in @p offset microseconds.
 * @return  UINT64_MAX in case of if the time in @p offset microseconds is
 *          is greater than UINT64_MAX.
 */
uint64_t extimer_get_absolute(uint64_t offset);

/**
 * @brief   Adds a new event to a timer
 *
 * Event fires immediately if its time is in the past.
 *
 * @pre timer != NULL
 * @pre extimer_event_t::next of event needs to be NULL.
 * @pre pid != KERNEL_PID_UNDEF
 *
 * @param[in] timer A timer. Must not be NULL.
 * @param[in] event An event. extimer_event_t::next must be NULL.
 * @param[in] pid   PID of the event handler thread.
 */
void extimer_add(extimer_t *timer, extimer_event_t *event, kernel_pid_t pid);

/**
 * @brief   Removes an existing event from a timer
 *
 * @pre timer != NULL
 * @pre event != NULL
 *
 * @param[in] timer A timer. Must not be NULL.
 * @param[in] event An event. Must not be NULL.
 *
 * @return  The removed event.
 * @return  NULL, if event list of @p timer was empty.
 */
extimer_event_t *extimer_del(extimer_t *timer, extimer_event_t *event);

#ifdef __cplusplus
}
#endif

#endif /* EXTIMER_H_ */
/** @} */
