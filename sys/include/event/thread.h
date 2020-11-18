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
 * Usage
 * =====
 *
 * By using the module `event_thread`, the event queues
 * @ref EVENT_PRIO_HIGHEST, @ref EVENT_PRIO_MEDIUM, and
 * @ref EVENT_PRIO_LOWEST are provided and declared in the header
 * `event/thread.h`. With default settings, the `auto_init` module will
 * automatically start one or more threads to handle these
 * queues.
 *
 * By default, a single thread with priority `EVENT_THREAD_MEDIUM_PRIO`
 * will handle all three event queues according to their priority.
 * An already started event handler will not be preempted in this case by an
 * incoming higher priority event. Still, lower priority event queues will only
 * be worked on once the higher priority queues are empty. Hence, the worst case
 * latency is increased by the worst case duration of any possible lower
 * priority event in this configuration.
 *
 * By using module `event_thread_highest`, the highest priority queue gets its
 * own thread. With this, events of the highest priority can preempt already
 * running event handlers of medium and lowest priority.
 *
 * By using module `event_thread_medium`, the lowest priority events are handled
 * in their own thread. With this, events of at least medium priority can
 * preempt already running events of the lowest priority.
 *
 * By using both module `event_thread_highest` and `event_thread_medium`, each
 * event queue gets its own thread. So higher priority events will always
 * preempt events of lower priority in this case.
 *
 * Finally, the module `event_thread_lowest` is provided for backward
 * compatibility and has no effect.
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
 *          handling multiple queues
 *
 * @param[in]   queues          array of the preallocated queue objects
 * @param[in]   queues_numof    number of elements in @p queues
 * @param[in]   stack           ptr to stack space
 * @param[in]   stack_size      size of stack
 * @param[in]   priority        priority to use
 *
 * @pre     @p queues_numof is at most 4
 */
void event_thread_init_multi(event_queue_t *queues, size_t queues_numof,
                             char *stack, size_t stack_size,
                             unsigned priority);

/**
 * @brief   Convenience function for initializing an event queue thread
 *
 * @param[in]   queue       ptr to preallocated queue object
 * @param[in]   stack       ptr to stack space
 * @param[in]   stack_size  size of stack
 * @param[in]   priority    priority to use
 */
static inline void event_thread_init(event_queue_t *queue,
                                     char *stack, size_t stack_size,
                                     unsigned priority)
{
    event_thread_init_multi(queue, 1, stack, stack_size, priority);
}

/**
 * @brief   Event queue priorities
 *
 * @details The lower the numeric value, the higher the priority. The highest
 *          priority is 0, so that these priorities can be used as index to
 *          access arrays.
 */
enum {
    EVENT_QUEUE_PRIO_HIGHEST,   /**< Highest event queue priority */
    EVENT_QUEUE_PRIO_MEDIUM,    /**< Medium event queue priority */
    EVENT_QUEUE_PRIO_LOWEST,    /**< Lowest event queue priority */
    EVENT_QUEUE_PRIO_NUMOF      /**< Number of event queue priorities */
};

extern event_queue_t event_thread_queues[EVENT_QUEUE_PRIO_NUMOF];

/**
 * @brief   Pointer to the event queue handling highest priority events
 */
#define EVENT_PRIO_HIGHEST  (&event_thread_queues[EVENT_QUEUE_PRIO_HIGHEST])
/**
 * @brief   Pointer to the event queue handling medium priority events
 */
#define EVENT_PRIO_MEDIUM   (&event_thread_queues[EVENT_QUEUE_PRIO_MEDIUM])
/**
 * @brief   Pointer to the event queue handling lowest priority events
 */
#define EVENT_PRIO_LOWEST   (&event_thread_queues[EVENT_QUEUE_PRIO_LOWEST])

#ifdef __cplusplus
}
#endif
#endif /* EVENT_THREAD_H */
/** @} */
