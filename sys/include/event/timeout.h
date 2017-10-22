/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Provides functionality to trigger events after timeout
 *
 * event_timeout intentionally does't extend event structures in order to
 * support events that are integrated in larger structs intrusively.
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * event_timeout_t event_timeout;
 *
 * printf("posting timed callback with timeout 1sec\n");
 * event_timeout_init(&event_timeout, &queue, (event_t*)&event);
 * event_timeout_set(&event_timeout, 1000000);
 * [...]
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event Timeout API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef EVENT_TIMEOUT_H
#define EVENT_TIMEOUT_H

#include "event.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeout Event structure
 */
typedef struct {
    xtimer_t timer;         /**< xtimer object used for timeout */
    event_queue_t *queue;   /**< event queue to post event to   */
    event_t *event;         /**< event to post after timeout    */
} event_timeout_t;

/**
 * @brief   Initialize timeout event object
 *
 * @param[in]   event_timeout   event_timeout object to initilalize
 * @param[in]   queue           queue that the timed-out event will be added to
 * @param[in]   event           event to add to queue after timeout
 */
void event_timeout_init(event_timeout_t *event_timeout, event_queue_t *queue, event_t *event);

/**
 * @brief   Set a timeout
 *
 * This will make the event as configured in @p event_timeout be triggered
 * after @p timeout miliseconds.
 *
 * @note: the used event_timeout struct must stay valid until after the timeout
 *        event has been processed!
 *
 * @param[in]   event_timeout   event_timout context onject to use
 * @param[in]   timeout         timeout in miliseconds
 */
void event_timeout_set(event_timeout_t *event_timeout, uint32_t timeout);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_TIMEOUT_H */
/** @} */
