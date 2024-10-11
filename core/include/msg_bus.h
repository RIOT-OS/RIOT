/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 *
 * @experimental
 *
 * @{
 *
 * @file
 * @brief       Messaging Bus API for inter process message broadcast.
 *
 *              Threads can subscribe to messages on one or multiple buses.
 *              If a message is posted on one bus, all threads that are
 *              subscribed to that message type will receive the message.
 *
 *              On one bus only 32 different message types are possible.
 *              Do not use the `type` and `sender_pid` fields of a message
 *              directly if it may have been received over a message bus.
 *              Instead, use the @ref msg_bus_get_type and
 *              @ref msg_bus_get_sender_pid functions.
 *
 *              If you want to check if a message was sent directly (not
 *              over a bus, you can use the @ref msg_is_from_bus function.
 *
 *              @note Make sure to unsubscribe from all previously subscribed
 *              buses before terminating a thread.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef MSG_BUS_H
#define MSG_BUS_H

#include <assert.h>
#include <stdint.h>

#include "list.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A message bus is just a list of subscribers.
 */
typedef struct {
    list_node_t subs;       /**< List of subscribers to the bus */
    uint16_t id;            /**< Message Bus ID */
} msg_bus_t;

/**
 * @brief Message bus subscriber entry.
 *        Should not be modified by the user.
 */
typedef struct {
    list_node_t next;       /**< next subscriber */
    uint32_t event_mask;    /**< Bitmask of event classes */
    kernel_pid_t pid;       /**< Subscriber PID */
} msg_bus_entry_t;

/**
 * @brief Flag set on `sender_pid` of `msg_t` that indicates that
 *        the message was sent over a bus.
 */
#define MSB_BUS_PID_FLAG    (1U << ((8 * sizeof(kernel_pid_t)) - 1))

/**
 * @brief Initialize a message bus.
 *
 * Must be called by the owner of a ``msg_bus_t`` struct.
 *
 * Message buses are considered to be long-running and must be
 * created before any threads can attach to them.
 *
 * There can be a maximum number of 2047 buses in total.
 */
void msg_bus_init(msg_bus_t *bus);

/**
 * @brief Get the message type of a message bus message.
 *
 * The `type` field of the`msg_t` also encodes the message bus ID,
 * so use this function to get the real 5 bit message type.
 *
 * If the message was not sent over a bus, this will return the
 * original message ID.
 *
 * @param[in] msg           A message that was received over a bus
 *
 * @return                  The message type
 */
static inline uint16_t msg_bus_get_type(const msg_t *msg)
{
    if (msg->sender_pid & MSB_BUS_PID_FLAG) {
        return msg->type & 0x1F;
    }
    else {
        return msg->type;
    }
}

/**
 * @brief Get the sender PID of a message bus message.
 *
 * The `sender_pid` field of the`msg_t` has a flag bit set
 * to indicate the message was sent over a bus, thus it should
 * not be used directly.
 *
 * @param[in] msg           A message that was received over a bus
 *
 * @return                  The sender pid
 */
static inline kernel_pid_t msg_bus_get_sender_pid(const msg_t *msg)
{
    return msg->sender_pid & ~MSB_BUS_PID_FLAG;
}

/**
 * @brief Check if a message originates from a bus
 *
 * If a thread is attached to multiple buses, this function can be used
 * to determine if a message originated from a certain bus.
 *
 * @param[in] bus           The bus to check for, may be NULL
 * @param[in] msg           The received message
 *
 * @return                  True if the messages @p m was sent over @p bus
 *                          If @p bus is NULL, this function returns true
 *                          if the message was sent over *any* bus.
 *                          False if the messages @p m was a direct message
 *                          or from a different bus.
 */
static inline bool msg_is_from_bus(const msg_bus_t *bus, const msg_t *msg)
{
    if (bus != NULL && (bus->id != (msg->type >> 5))) {
        return false;
    }

    return msg->sender_pid & MSB_BUS_PID_FLAG;
}

/**
 * @brief Attach a thread to a message bus.
 *
 * This attaches a message bus subscriber entry to a message bus.
 * Subscribe to events on the bus using @ref msg_bus_detach.
 * The thread will then receive events with a matching type that are
 * posted on the bus.
 *
 * Events can be received with @ref msg_receive.
 * **The contents of the received message must not be modified.**
 *
 * @param[in] bus           The message bus to attach to
 * @param[in] entry         Message bus subscriber entry
 */
void msg_bus_attach(msg_bus_t *bus, msg_bus_entry_t *entry);

/**
 * @brief Remove a thread from a message bus.
 *
 * This removes the calling thread from the message bus.
 *
 * @note Call this function before the thread terminates.
 *
 * @param[in] bus           The message bus from which to detach
 * @param[in] entry         Message bus subscriber entry
 */
void msg_bus_detach(msg_bus_t *bus, msg_bus_entry_t *entry);

/**
 * @brief Get the message bus entry for the current thread.
 *
 * Traverse the message bus to find the subscriber entry for the
 * current thread.
 *
 * @param[in] bus           The message bus to search
 *
 * @return                  The subscriber entry for the current thread.
 *                          NULL if the thread is not attached to @p bus.
 */
msg_bus_entry_t *msg_bus_get_entry(msg_bus_t *bus);

/**
 * @brief Subscribe to an event on the message bus.
 *
 * @pre The @p entry has been attached to a bus with @ref msg_bus_attach.
 *
 * @param[in] entry         The message bus entry
 * @param[in] type          The event type to subscribe to (range: 0…31)
 */
static inline void msg_bus_subscribe(msg_bus_entry_t *entry, uint8_t type)
{
    assert(type < 32);
    entry->event_mask |= (1UL << type);
}

/**
 * @brief Unsubscribe from an event on the message bus.
 *
 * @pre The @p entry has been attached to a bus with @ref msg_bus_attach.
 *
 * @param[in] entry         The message bus entry
 * @param[in] type          The event type to unsubscribe (range: 0…31)
 */
static inline void msg_bus_unsubscribe(msg_bus_entry_t *entry, uint8_t type)
{
    assert(type < 32);
    entry->event_mask &= ~(1UL << type);
}

/**
 * @brief Post a pre-assembled message to a bus.
 *
 * This function sends a message to all threads listening on the bus which are
 * listening for messages with the message type of @p m.
 *
 * It behaves identical to @ref msg_bus_post, but sends a pre-defined message.
 *
 * @note The message is expected to have the event ID encoded in the lower
 *       5 bits and the bus ID encoded in the upper 11 bits of the message type.
 *
 * @param[in] m             The message to post the bus
 * @param[in] bus           The message bus to post the message on
 *
 * @return                  The number of threads the message was sent to.
 */
int msg_send_bus(msg_t *m, msg_bus_t *bus);

/**
 * @brief Post a message to a bus.
 *
 * This function sends a message to all threads listening on the bus which are
 * listening for messages of @p type.
 *
 * It is safe to call this function from interrupt context.
 *
 * @param[in] bus           The message bus to post this on
 * @param[in] type          The event type (range: 0…31)
 * @param[in] arg           Optional event parameter
 *
 * @return                  The number of threads the event was posted to.
 */
static inline int msg_bus_post(msg_bus_t *bus, uint8_t type, const void *arg)
{
    assert(type < 32);

    msg_t m = {
        .type = type | ((bus->id) << 5),
        .content.ptr = (void *)arg,
    };

    return msg_send_bus(&m, bus);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MSG_BUS_H */
