/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Provides a callback-with-argument event type
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * void callback(void *arg)
 * {
 *     printf("%s called with arg %p\n", __func__, arg);
 * }
 *
 * [...]
 * event_callback_t event_callback = EVENT_CALLBACK_INIT(callback, 0x12345678);
 * event_post(&queue, &event_callback);
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Event Callback API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef EVENT_CALLBACK_H
#define EVENT_CALLBACK_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback Event structure definition
 */
typedef struct {
    event_t super;              /**< event_t structure that gets extended   */
    void (*callback)(void*);    /**< callback function                      */
    void *arg;                  /**< callback function argument             */
} event_callback_t;

/**
 * @brief   event callback initialization function
 *
 * @param[out]  event_callback  object to initialize
 * @param[in]   callback        callback to set up
 * @param[in]   arg             callback argument to set up
 */
void event_callback_init(event_callback_t *event_callback, void (*callback)(void *), void *arg);

/**
 * @brief   event callback handler function (used internally)
 *
 * @internal
 *
 * @param[in]   event   callback event to process
 */
void _event_callback_handler(event_t *event);

/**
 * @brief   Callback Event static initializer
 *
 * @param[in]   _cb     callback function to set
 * @param[in]   _arg    arguments to set
 */
#define EVENT_CALLBACK_INIT(_cb, _arg) \
    { \
        .super.handler = _event_callback_handler, \
        .callback = _cb, \
        .arg = (void *)_arg \
    }

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CALLBACK_H */
/** @} */
