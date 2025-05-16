/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_event
 * @brief       Provides functionality to trigger multiple events at once
 *
 * @{
 *
 * @file
 * @brief       Event Source API
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 */


#include "event.h"
#include "list.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Event source struct
 */
typedef struct {
    list_node_t list;       /**< event source list node */
} event_source_t;

/**
 * @brief   Subscriber of an event source
 */
typedef struct {
    list_node_t node;       /**< event subscriber list node  */
    event_t *event;         /**< pointer to event that should be triggered */
    event_queue_t *queue;   /**< event queue to be used for the event */
} event_source_subscriber_t;

/**
 * @brief   Initialize an event source
 */
#define EVENT_SOURCE_INIT  { 0 }

/**
 * @brief   Initialize an event source subscriber
 *
 * @param[in]   e       The event to trigger
 * @param[in]   q       The event queue to use for the event
 */
#define EVENT_SOURCE_SUBSCRIBER_INIT(e, q) { .event = (event_t *)e, .queue = q }

/**
 * @brief   Attach an event to an event source
 *
 * @param[in]   source  The event list to attach the event to
 * @param[in]   event   The new event subscription
 */
static inline void event_source_attach(event_source_t *source,
                                       event_source_subscriber_t *event)
{
    unsigned state = irq_disable();

    list_add(&source->list, &event->node);

    irq_restore(state);
}

/**
 * @brief   Remove a subscription from the event source
 *
 * @param[in]   source  The event list to remove the event from
 * @param[in]   event   The event to remove
 */
static inline void event_source_detach(event_source_t *source,
                                       event_source_subscriber_t *event)
{
    unsigned state = irq_disable();

    list_remove(&source->list, &event->node);

    irq_restore(state);
}

/**
 * @brief   Trigger all events registered on the event source
 *
 * @param[in]   source  The event list to trigger
 */
static inline void event_source_trigger(event_source_t *source)
{
    unsigned state = irq_disable();

    event_source_subscriber_t *event = container_of(source->list.next,
                                                    event_source_subscriber_t,
                                                    node);
    while (event) {
        event_post(event->queue, event->event);
        event = container_of(event->node.next, event_source_subscriber_t, node);
    }

    irq_restore(state);
}

#ifdef __cplusplus
}
#endif
/** @} */
