/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "ztimer.h"
#include "random.h"
#include "event.h"
#include "container.h"

#include "net/unicoap/server.h"

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

#if DEVELHELP || defined (DOXYGEN)
    /**
     * @brief Null-terminated event identifier used in debug logs
     *
     * Requires `DEVELHELP`
     */
    const char* name;
#endif
} unicoap_scheduled_event_t;

/**
 * @brief Name of scheduled event
 * @returns Null-terminated string when `DEVELHELP` is turned on, `NULL` otherwise
 * @param event Scheduled event
 */
static inline const char* unicoap_scheduled_event_name(unicoap_scheduled_event_t* event) {
    (void)event;
#if DEVELHELP
    return event->name;
#else
    return NULL;
#endif
}

/**
 * @brief Returns scheduled event subclass of event
 * @param[in] event Superclass event
 * @returns Scheduled event
 */
static inline unicoap_scheduled_event_t* unicoap_scheduled_event_of_event(event_t* event) {
    return container_of(event, unicoap_scheduled_event_t, super);
}

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
 * to the parent structure.
 *
 * @param[in] event pointer to event
 */
typedef void (*unicoap_event_callback_t)(unicoap_scheduled_event_t* event);

/**
 * @brief Schedules an event on the internal unicoap queue
 *
 * If you instead want to execute as soon as possible, use @ref unicoap_loop_enqueue instead.
 *
 * @param[in,out] event The event to schedule. Provide a pointer to a pre-allocated event
 * @param[in] callback Function pointer to be called on the internal queue after @p duration ms have elapses
 * @param duration Number of milliseconds to wait
 * @param[in] name A null-terminated string identifier to distinguish the event in debug logs
 *
 * @p name has no effect when `DEVELHELP` is disabled.
 */
void unicoap_event_schedule(unicoap_scheduled_event_t* event, unicoap_event_callback_t callback,
                            uint32_t duration, const char* name);

/**
 * @brief Discards the currently set timeout, and reschedules the event to be posted in @p duration ms
 *
 * Use this method to rerun the timer with the same callbacks but a new timeout value.
 * E.g., the RFC 7252 driver utilizes this method for setting the next acknowledgement timeout.
 *
 * @param[in] event Scheduled event you want to reschedule
 * @param[in] duration Number of milliseconds the event should be posted on the queue
 */
void unicoap_event_reschedule(unicoap_scheduled_event_t* event, uint32_t duration);
/**
 * @brief Cancels the event
 *
 * @param[in] event Scheduled event you want to cancel
 *
 * Internally, the timer is removed from the clock.
 *
 * @note If the event has already been posted on the queue, this method will try to remove
 * the cancel the event that has already been posted to the queue.
 */
void unicoap_event_cancel(unicoap_scheduled_event_t* event);

/**
 * @brief A signal sent from one layer to another indicating a change to a state object
 *
 * @see @ref UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE
 * @see @ref UNICOAP_LAYER_NOTIFICATION_STATE_ALLOC
 * @see @ref UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE
 * @see @ref unicoap_layer_notification_async_failure_to_errno
 * @see @ref unicoap_layer_notification_async_failure_from_errno
 */
typedef int unicoap_layer_notification_t;

/**
 * @brief Event indicating a failure on the layer this message is sent from
 *
 * The recipient layer should try to release state objects as soon as possible.
 *
 * @warning **Only notify the exchange layer of errors that were produced on the messaging layer and
 * that happened asynchronously (not while in a call frame from the exchange layer).** Synchronous
 * errors shall be reported by returning negative integers.
 * @see @ref unicoap_exchange_notify.
 */
#define UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE (~(~0U >> 1))

/**
 * @brief Converts notification to negative error number
 * @param type Notification
 * @return Negative integer indicating error
 */
static inline int unicoap_layer_notification_async_failure_to_errno(unicoap_layer_notification_t type) {
    assert(type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE);
    assert(type < 0);
    return type;
}

/**
 * @brief Converts error number into notification
 * @param error Positive or negative integer indicating error, e.g., `-ETIMEDOUT` or `ETIMEDOUT`
 * @returns Notification
 */
static inline unicoap_layer_notification_t unicoap_layer_notification_async_failure_from_errno(int error) {
    return -abs(error);
}

/**
 * @brief Event indicating a layer is finished and is releasing its allocated
 *        state objects of this exchange/transmission
 *
 * The recipient layer must determine whether it still needs to retain its allocated
 * state objects.
 */
#define UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE (0)

/**
 * @brief Event indicating a layer is allocating a state object
 *
 * The recipient layer can attach state it itself allocated.
 */
#define UNICOAP_LAYER_NOTIFICATION_STATE_ALLOC (1)

/**
 * @brief Informs messaging layer of event
 *
 * @param state Messaging-layer state reference
 * @param type Event type
 * @param[in] arg Optional opaque state object in this layer the notification relates to.
 * @param proto The protocol number for the underlying CoAP driver
 *
 * Usually called from exchange layer.
 */
void unicoap_messaging_notify(void* state, unicoap_layer_notification_t type,
                              void* arg, unicoap_proto_t proto);

/**
 * @brief Informs exchange layer of event
 *
 * @param state Exchange-layer state reference
 * @param type Event type
 * @param[in] arg Optional opaque state object in this layer the notification relates to.
 *
 * Usually called from messaging layer. Do not notify the exchange layer of failures that occur
 * in a synchronous call from the exchange layer into the messaging layer (`send`) or that originate
 * from the exchange layer (`preprocess` and `process`). Only propagate errors that arise
 * asynchronously, e.g., negative acknowledgement or connection resets directly on the messaging
 * layer.
 *
 * @warning **The messaging layer must not send a notification of type
 * @ref UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE before @ref unicoap_messaging_send has returned!**
 * In this case, @ref unicoap_messaging_send shall return a negative integer indicating an error
 * instead, such as `-ENOBUFS` or `-ECONNABORTED`.
 *
 * @warning **The messaging layer must also not send a notification of type
 * @ref UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE when @ref unicoap_exchange_preprocess or
 * @ref unicoap_exchange_process already returned an error for the same incoming message!**
 * In these cases, the exchange layer will take care of handling the error, and the messaging layer
 * must not notify the exchange layer of this error again.
 */
void unicoap_exchange_notify(void* state, unicoap_layer_notification_t type, void* arg);

/**
 * @brief Informs exchange layer of event that applies to all exchange state objects associated
 * with given endpoint
 * @param endpoint Endpoint
 * @param type Event type
 * @param[in] arg Optional opaque state object in this (sending) layer the notification relates to.
 *
 * Usually called from messaging layer.
 */
void unicoap_exchange_notify_all(const unicoap_endpoint_t* endpoint, unicoap_layer_notification_t type, void* arg);

/** @} */

/* TODO: Client and advanced server features: Elaborate state management */

#ifdef __cplusplus
}
#endif

/** @} */
