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
} unicoap_scheduled_event_t;

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
 * @param[in,out] event The event to schedule. Provide a pointer to a pre-allocated event
 * @param[in] callback Function pointer to be called on the internal queue after @p duration ms have elapses
 * @param duration Number of milliseconds to wait
 */
void unicoap_event_schedule(unicoap_scheduled_event_t* event, unicoap_event_callback_t callback,
                            uint32_t duration);

/**
 * @brief Discards the currently set timeout, and reschedules the event to be posted in @p duration ms
 *
 * Use this method to rerun the timer with the same callbacks but a new timeout value.
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
 * the cancel the event that has already been posted to the queue.
 */
void unicoap_event_cancel(unicoap_scheduled_event_t* event);
/** @} */

/* MARK: - Common state */
/**
 * @name Common state
 * @{
 */

/** @brief A feature check macro that determines whether there's at least one driver that stores
*          messaging-layer state for transmissions. */
#define UNICOAP_HAVE_MESSAGING_STATE \
    IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
/* MARK: unicoap_driver_extension_point */

/**
 * @brief A type used to retain state spanning across a single or multiple message exchanges
 *
 * A memo is the central bucket or bundle carrying an endpoint, exchange, and messaging data.
 * This struct is reused for multiple consecutive request-response pairs, such as those found in a
 * Block-wise exchange.
 */
typedef struct {
    /**
     * @brief Exchange state
     *
     * State tracked by the request-response layer, i.e., client requests, block-wise transfers,
     * and deferred responses.
     */
    struct {
        /**
         * @brief Timeout
         */
        unicoap_scheduled_event_t timeout;
    } exchange;

    /**
     * @brief Messaging state
     *
     * @note There's no union for the underlying pointer type as drivers are allowed to keep their
     *       data structures
     * private. This design avoids the need to expose every driver data structure.
     */
    union {
#if UNICOAP_HAVE_MESSAGING_STATE || defined(DOXYGEN)

        /**
         * @brief Messaging state
         */
        void* state;
#endif
    } messaging;

    /**
     * @brief The remote CoAP endpoint unicoap maintains an exchange with (client/server) or
     * stores messaging state for.
     */
    unicoap_endpoint_t endpoint;
} unicoap_memo_t;

/**
 * @brief Returns messaging state associated with memo
 * @param memo Client state object
 * @return Messaging state object pointer
 */
static inline void* unicoap_memo_messaging_state(unicoap_memo_t* memo) {
#if UNICOAP_HAVE_MESSAGING_STATE
    return memo->messaging.state;
#else
    return NULL;
#endif
}

/**
 * @brief Returns memo of scheduled event
 * @param[in] timeout Scheduled timeout event
 * @returns Common memo state object
 */
static inline unicoap_memo_t* unicoap_memo_of_timeout(unicoap_scheduled_event_t* timeout) {
    return container_of(timeout, unicoap_memo_t, exchange.timeout);
}

/**
 * @brief Returns memo of scheduled event
 * @param[in] event Superclass event
 * @returns Common memo state object
 */
static inline unicoap_memo_t* unicoap_memo_of_event(event_t* event) {
    return unicoap_memo_of_timeout(unicoap_scheduled_event_of_event(event));
}

typedef int unicoap_layer_notification_t;

/**
 * @brief Event indicating a failure on the layer this message is sent from
 *
 * The recipient layer should try to release state objects as soon as possible.
 *
 * @warning **Only notify the exchange layer of errors that were produced on the messaging layer and
 * that happened asynchronously (not while in a call frame from the exchange layer).**
 * @see @ref unicoap_exchange_notify.
 */
#define UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE (~(~0U >> 1))

static inline int unicoap_layer_notification_async_failure_to_errno(unicoap_layer_notification_t type) {
    assert(type & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE);
    assert(type < 0);
    return type;
}

static inline unicoap_layer_notification_t unicoap_layer_notification_async_failure_from_errno(int error) {
    assert(error > 0);
    assert((-error) & UNICOAP_LAYER_NOTIFICATION_ASYNC_FAILURE);
    return -error;
}

/**
 * @brief Event indicating a layer is finished and is releasing its allocated
 * state objects of this exchange/transmission
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
 * @param state Messaging-layer state reference
 * @param type Event type
 * @param[in] arg Optional opaque state object in this layer the notification relates to.
 * @param proto The protocol number for the underlying CoAP driver
 *
 * Usually called from exchange layer
 */
void unicoap_messaging_notify(void* state, unicoap_layer_notification_t type, void* arg, unicoap_proto_t proto);

/**
 * @brief Informs exchange layer of event
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

/* MARK: - Client state */
/**
 * @name Client state
 * @{
 */
/**
 * @brief A type capable of representing either a response or block-wise callback
 * @private
 */
typedef union {
    /** @brief Called once the response is received. Used by the client. */
    unicoap_response_callback_t response;
    void* _any;
} unicoap_callback_t;

/**
 * @brief Determines whether the given callback is not `NULL`
 * @param[in] callback Callbacks
 * @returns A boolean value indicating whether the given callback is NULL
 */
static inline bool unicoap_callback_is_present(const unicoap_callback_t callback) {
    /* Per ISO-C, union members all start at the same address, hence checking if
     the first member is NULL is fine as long as the second member also is a pointer.
     Both are function pointers. Global reasoning says we're fine here. */
    return callback._any != NULL;
}

/**
 * @brief Client exchange
 */
typedef struct {
    unicoap_memo_t super;

    /** @brief Callback function registered by the client API */
    unicoap_callback_t callback;

    /** @brief Argument to be passed to @p callback */
    void* callback_arg;

    /** @brief Request token generated by unicoap */
    uint8_t token[CONFIG_UNICOAP_GENERATED_TOKEN_LENGTH];

#if IS_ACTIVE(CONFIG_UNICOAP_CLIENT_CANCELLABLE)
    uint16_t reference_id;
#endif

    /**
     * @brief Request flags from application, from the original call to `unicoap_send_request_*
     */
    unicoap_client_flags_t flags;
} unicoap_client_memo_t;

/**
 * @brief Returns client memo of common memo state object
 * @param[in] memo Superclass memo
 * @returns Client memo state object
 */
static inline unicoap_client_memo_t* unicoap_client_memo_of_super(unicoap_memo_t* memo) {
    return container_of(memo, unicoap_client_memo_t, super);
}

/**
 * @brief Returns client memo of event
 * @param[in] event Superclass event
 * @returns Client memo state object
 */
static inline unicoap_client_memo_t* unicoap_client_memo_of_event(event_t* event) {
    return unicoap_client_memo_of_super(unicoap_memo_of_event(event));
}

/**
 * @brief Returns client memo of scheduled event
 * @param[in] event Superclass event
 * @returns Client memo state object
 */
static inline unicoap_client_memo_t* unicoap_client_memo_of_timeout(unicoap_scheduled_event_t* timeout) {
    return unicoap_client_memo_of_super(unicoap_memo_of_timeout(timeout));
}

/**
 * @brief Invokes the client callback
 *
 * @param[in,out] memo Client state object
 * @param[in] packet A packet containing the response to the request
 * @param block `Block` option, used to choose correct callback prototype
 */
int unicoap_client_callback_success(unicoap_client_memo_t* memo, const unicoap_packet_t* packet,
                                    unicoap_block_option_t block);

/**
 * @brief Invokes the client callback indicating an error occurred
 */
void unicoap_client_callback_failure(unicoap_client_memo_t* memo, int error);

/**
 * @brief Allocates and sets up a new memo
 *
 * @note This function is thread-safe.
 *
 * @param[in] endpoint Remote endpoint
 *
 * @returns New memo
 */
unicoap_client_memo_t* unicoap_client_memo_create(const unicoap_endpoint_t* endpoint);

/**
 * @brief Tries to find a client exchange memo by endpoint and token
 *
 * @param[in] endpoint Remote endpoint of exchange
 * @param[in] token Token
 * @param token_length Token length
 *
 * @returns Memo, if found
 */
unicoap_client_memo_t* unicoap_client_memo_find_token(const unicoap_endpoint_t* endpoint,
                                                  const uint8_t* token, size_t token_length);


/**
 * @brief Tries to find client state object based on reference number
 *
 * @param refno Reference number associated with client memo
 * @returns Client state object or `NULL` if memo is no longer associated with reference number
 * 
 * This function may be used to find client exchange-layer state objects without keeping a pointer
 * to it. This is useful for handing public API callers a reference to, e.g., cancellable state
 * objects. With solely a pointer to the memo, `unicoap` would be at risk of accessing a memo
 * that had subsequently been released and then got reused, thus no longer relates to the original
 * exchange. Relying on the reference number prevents this as it contains a fingerprint of the
 * memo contents. Should the memo get released, obsolete references can be detected and discarded.
 */
unicoap_client_memo_t* unicoap_client_memo_find_refno(int refno);

int unicoap_client_memo_assign_refno(unicoap_client_memo_t* memo);

/**
 * @brief Frees memo and associated buffers
 *
 * @param[in,out] memo Memo state bucket to discard and free
 */
void unicoap_client_memo_free(unicoap_client_memo_t* memo);
/** @} */

/* TODO: Client and advanced server features: Elaborate state management */

#ifdef __cplusplus
}
#endif

/** @} */
