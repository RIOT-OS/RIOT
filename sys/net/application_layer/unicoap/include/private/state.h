/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include "ztimer.h"
#include "random.h"
#include "event.h"
#include "container.h"

#include "net/unicoap/application.h"

#include "private/packet.h"

/**
 * @defgroup net_unicoap_private_state State Management
 * @ingroup  net_unicoap_private
 * @{
 */

/**
 * @file
 * @brief  State and Memo API
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An event scheduled for a specified point in time
 * @ingroup net_unicoap_private_state
 * @private
 *
 * @note The API for this structure is internal.
 *
 * In contrast to @ref event_timeout_t this structure does not store the queue and clock as they
 * are known statically and remain the same. This structure also does not use an @ref event_callback_t
 * to save the space needed to store an additional function pointer.
 *
 * # Sequence of Events
 * ```
 *      You schedule an event
 *               \/
 *     event_t is initialized
 *         ztimer_t is set
 *               \/
 *              ....
 *           Timer fires
 *               \/
 *   Internal callback is called
 *               \/
 *    Event is posted on queue
 *               \/
 *              ....
 *     Queue calls your callback
 *
 * Fig. 1: Sequence of events
 * ```
 */
typedef struct {
    /**
     * @brief Event to be posted to the internal event queue
     * @internal
     *
     * This event stores the callback you provided to @ref unicoap_event_schedule
     */
    event_t super;

    /**
     * @brief Timer used to post the event
     * @internal
     *
     * The timer stores the internal callback that posts the @ref unicoap_scheduled_event_t.super
     * event on the internal unicoap queue.
     */
    ztimer_t ztimer;
} unicoap_scheduled_event_t;

/**
 * @brief Memo typealias
 */
typedef struct unicoap_memo unicoap_memo_t;

/* MARK: - Event Scheduling */
/**
 * @name Event Scheduling
 * @{
 */
/** @brief The ztimer clock used for event scheduling */
#define UNICOAP_CLOCK ZTIMER_MSEC

/**
 * @brief Scheduled event callback
 *
 * Use the @p event parameter and the @ref container_of macro to get a pointer
 * the parent structure.
 *
 * @param[in] event pointer to event
 */
typedef void (*unicoap_event_callback_t)(unicoap_scheduled_event_t* event);

/**
 * @brief Schedules an event on the internal unicoap queue
 *
 * @param[in,out] event The event to schedule. Provide a pointer to a pre-allocated event
 * @param[in] callback Function pointer to be called on the internal queue after @p duration ms have elapses
 * @param duration Number of milliseconds to wait
 */
void unicoap_event_schedule(unicoap_scheduled_event_t* event, event_handler_t callback,
                            uint32_t duration);

/**
 * @brief Discards the currently set timeout, and reschedules the event to be posted in @p duration ms
 *
 * Use this method rerun the timer with the same callbacks but a new timeout value.
 * E.g., the RFC 7252 driver utilizes this method for setting the next acknowledgement timeout.
 *
 * @param[in] event Scheduled event you want to reschedule
 * @param[in] duration Number of milliseconds the event should be posted on the queue
 */
static inline void unicoap_event_reschedule(unicoap_scheduled_event_t* event, uint32_t duration)
{
    ztimer_set(UNICOAP_CLOCK, &event->ztimer, duration);
}

/**
 * @brief Cancels the event
 *
 * Internally, the timer is removed from the clock.
 *
 * @note If the event has already been posted on the queue, this method will try to remove
 * the cancel the @ref event_t posted on the queue.
 */
void unicoap_event_cancel(unicoap_scheduled_event_t* event);
/** @} */

/* TODO: Client and advanced server features: Elaborate state management */

#ifdef __cplusplus
}
#endif

/** @} */
