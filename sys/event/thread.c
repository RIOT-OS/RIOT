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

#include "architecture.h"
#include "thread.h"
#include "event.h"
#include "event/thread.h"

struct event_queue_and_size {
    event_queue_t *q;
    size_t q_numof;
};

static void *_handler_thread(void *tagged_ptr)
{
    event_queue_t *qs = ptrtag_ptr(tagged_ptr);
    /* number of queues is encoded in lower pointer bits */
    size_t n = ptrtag_tag(tagged_ptr) + 1;
    event_queues_claim(qs, n);
    /* start event loop */
    event_loop_multi(qs, n);

    /* should be never reached */
    return NULL;
}

void event_thread_init_multi(event_queue_t *queues, size_t queues_numof,
                             char *stack, size_t stack_size, unsigned priority)
{
    /* For the auto_init use case, this will be called before main gets
     * started.  main might already use the queues, so they need to be
     * initialized at that point.
     *
     * They will be claimed within the handler thread.
     */
    event_queues_init_detached(queues, queues_numof);

    void *tagged_ptr = ptrtag(queues, queues_numof - 1);

    thread_create(stack, stack_size, priority, 0,
                  _handler_thread, tagged_ptr, "event");
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

/* rely on compiler / linker to garbage collect unused stacks */
static char WORD_ALIGNED _evq_highest_stack[EVENT_THREAD_HIGHEST_STACKSIZE];
static char WORD_ALIGNED _evq_medium_stack[EVENT_THREAD_MEDIUM_STACKSIZE];
static char WORD_ALIGNED _evq_lowest_stack[EVENT_THREAD_LOWEST_STACKSIZE];

event_queue_t event_thread_queues[EVENT_QUEUE_PRIO_NUMOF];

void auto_init_event_thread(void)
{
    if (IS_USED(MODULE_EVENT_THREAD_HIGHEST)) {
        /* In order to allow highest priority events to preempt all others,
         * high priority events must be run in their own thread. This thread
         * can preempt than preempt the other event thread(s). */
        event_thread_init(EVENT_PRIO_HIGHEST,
                          _evq_highest_stack, sizeof(_evq_highest_stack),
                          EVENT_THREAD_HIGHEST_PRIO);
    }
    if (IS_USED(MODULE_EVENT_THREAD_MEDIUM)) {
        /* In order to allow medium priority events to preempt low priority
         * events, we need to move the low priority events into their own
         * thread. The always existing medium priority event thread can then
         * preempt the lowest priority event thread. */
        event_thread_init(EVENT_PRIO_LOWEST,
                          _evq_lowest_stack, sizeof(_evq_lowest_stack),
                          EVENT_THREAD_LOWEST_PRIO);
    }

    event_queue_t *qs = EVENT_PRIO_MEDIUM;
    size_t qs_numof = 1;
    if (!IS_USED(MODULE_EVENT_THREAD_HIGHEST)) {
        qs = EVENT_PRIO_HIGHEST;
        qs_numof = 2;
    }
    if (!IS_USED(MODULE_EVENT_THREAD_MEDIUM)) {
        qs_numof++;
    }
    event_thread_init_multi(qs, qs_numof,
                            _evq_medium_stack, sizeof(_evq_medium_stack),
                            EVENT_THREAD_MEDIUM_PRIO);
}
