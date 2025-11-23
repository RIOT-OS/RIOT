/*
 * SPDX-FileCopyrightText: 2016 Sam Kumar <samkumar@berkeley.edu>
 * SPDX-FileCopyrightText: 2016 University of California, Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_sync
 * @{
 *
 * @file
 * @brief       Kernel condition variable implementation
 *
 * @author      Sam Kumar <samkumar@berkeley.edu>
 *
 * @}
 */

#include "cond.h"
#include "irq.h"
#include "mutex.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void cond_init(cond_t *cond)
{
    cond->queue.next = NULL;
}

void cond_wait(cond_t *cond, mutex_t *mutex)
{
    unsigned irqstate = irq_disable();
    thread_t *me = thread_get_active();

    mutex_unlock(mutex);
    sched_set_status(me, STATUS_COND_BLOCKED);
    thread_add_to_list(&cond->queue, me);
    irq_restore(irqstate);
    thread_yield_higher();

    /*
     * Once we reach this point, the condition variable was signalled,
     * and we are free to continue.
     */
    mutex_lock(mutex);
}

static void _cond_signal(cond_t *cond, bool broadcast)
{
    unsigned irqstate = irq_disable();
    list_node_t *next;

    uint16_t min_prio = THREAD_PRIORITY_MIN + 1;

    while ((next = list_remove_head(&cond->queue)) != NULL) {
        thread_t *process = container_of((clist_node_t *)next, thread_t,
                                         rq_entry);
        sched_set_status(process, STATUS_PENDING);
        uint16_t process_priority = process->priority;
        if (process_priority < min_prio) {
            min_prio = process_priority;
        }

        if (!broadcast) {
            break;
        }
    }

    irq_restore(irqstate);
    if (min_prio <= THREAD_PRIORITY_MIN) {
        sched_switch(min_prio);
    }
}

void cond_signal(cond_t *cond)
{
    _cond_signal(cond, false);
}

void cond_broadcast(cond_t *cond)
{
    _cond_signal(cond, true);
}
