/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Provides utility functions for event handler threads
 *
 * @{
 *
 * @file
 * @brief       Event Thread API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef EVENT_THREAD_H
#define EVENT_THREAD_H

#include <stddef.h>

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Convenience function for initializing an event queue thread
 *
 * @param[in]   queue       ptr to preallocated queue object
 * @param[in]   stack       ptr to stack space
 * @param[in]   stack_size  size of stack
 * @param[in]   priority    priority to use
 */
void event_thread_init(event_queue_t *queue, char *stack, size_t stack_size,
                       unsigned priority);

#ifdef MODULE_EVENT_THREAD_HIGHEST
extern event_queue_t event_queue_highest;
#define EVENT_PRIO_HIGHEST (&event_queue_highest)
#endif

#ifdef MODULE_EVENT_THREAD_MEDIUM
extern event_queue_t event_queue_medium;
#define EVENT_PRIO_MEDIUM (&event_queue_medium)
#endif

#ifdef MODULE_EVENT_THREAD_LOWEST
extern event_queue_t event_queue_lowest;
#define EVENT_PRIO_LOWEST (&event_queue_lowest)
#endif

#ifdef __cplusplus
}
#endif
#endif /* EVENT_THREAD_H */
/** @} */
