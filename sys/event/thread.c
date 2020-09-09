/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @{
 *
 * @file
 * @brief       Event Loop Thread implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "thread.h"
#include "event.h"
#include "event/thread.h"

static void *_handler(void *event_queue)
{
    event_queue_claim(event_queue);
    event_loop(event_queue);

    /* should be never reached */
    return NULL;
}

void event_thread_init(event_queue_t *queue, char *stack, size_t stack_size,
                       unsigned priority)
{
    /* For the auto_init use case, this will be called before main gets
     * started.  main might already use the queues, so they need to be
     * initialized at that point.
     *
     * They will be claimed within the handler thread.
     */
    event_queue_init_detached(queue);

    thread_create(stack, stack_size, priority, 0, _handler, queue, "event");
}

#ifndef EVENT_THREAD_STACKSIZE_DEFAULT
# ifdef ISR_STACKSIZE
#  define EVENT_THREAD_STACKSIZE_DEFAULT ISR_STACKSIZE
# else
#  define EVENT_THREAD_STACKSIZE_DEFAULT THREAD_STACKSIZE_SMALL
# endif
#endif

#ifndef EVENT_THREAD_HIGHEST_STACKSIZE
#define EVENT_THREAD_HIGHEST_STACKSIZE EVENT_THREAD_STACKSIZE_DEFAULT
#endif

#ifndef EVENT_THREAD_HIGHEST_PRIO
#define EVENT_THREAD_HIGHEST_PRIO   (0)
#endif

#ifndef EVENT_THREAD_MEDIUM_STACKSIZE
#define EVENT_THREAD_MEDIUM_STACKSIZE EVENT_THREAD_STACKSIZE_DEFAULT
#endif
#ifndef EVENT_THREAD_MEDIUM_PRIO
#define EVENT_THREAD_MEDIUM_PRIO   (THREAD_PRIORITY_MAIN - 1)
#endif

#ifndef EVENT_THREAD_LOWEST_STACKSIZE
#define EVENT_THREAD_LOWEST_STACKSIZE EVENT_THREAD_STACKSIZE_DEFAULT
#endif
#ifndef EVENT_THREAD_LOWEST_PRIO
#define EVENT_THREAD_LOWEST_PRIO   (THREAD_PRIORITY_IDLE - 1)
#endif

#ifdef MODULE_EVENT_THREAD_HIGHEST
event_queue_t event_queue_highest;
static char _evq_highest_stack[EVENT_THREAD_HIGHEST_STACKSIZE];
#endif

#ifdef MODULE_EVENT_THREAD_MEDIUM
event_queue_t event_queue_medium;
static char _evq_medium_stack[EVENT_THREAD_MEDIUM_STACKSIZE];
#endif

#ifdef MODULE_EVENT_THREAD_LOWEST
event_queue_t event_queue_lowest;
static char _evq_lowest_stack[EVENT_THREAD_LOWEST_STACKSIZE];
#endif

typedef struct {
    event_queue_t *queue;
    char *stack;
    size_t stack_size;
    unsigned priority;
} event_threads_t;

const event_threads_t _event_threads[] = {
#ifdef MODULE_EVENT_THREAD_HIGHEST
    { &event_queue_highest, _evq_highest_stack, sizeof(_evq_highest_stack),
        EVENT_THREAD_HIGHEST_PRIO },
#endif
#ifdef MODULE_EVENT_THREAD_MEDIUM
    { &event_queue_medium, _evq_medium_stack, sizeof(_evq_medium_stack),
        EVENT_THREAD_MEDIUM_PRIO },
#endif
#ifdef MODULE_EVENT_THREAD_LOWEST
    { &event_queue_lowest, _evq_lowest_stack, sizeof(_evq_lowest_stack),
        EVENT_THREAD_LOWEST_PRIO },
#endif
};

void auto_init_event_thread(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_event_threads); i++) {
        event_thread_init(_event_threads[i].queue,
                _event_threads[i].stack, _event_threads[i].stack_size,
                _event_threads[i].priority);
    }
}
