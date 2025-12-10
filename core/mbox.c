/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_mbox
 * @{
 *
 * @file
 * @brief       mailbox implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>

#include "mbox.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _wake_waiter(thread_t *thread, unsigned irqstate)
{
    sched_set_status(thread, STATUS_PENDING);

    DEBUG("mbox: Thread %" PRIkernel_pid ": _wake_waiter(): waking up "
          "%" PRIkernel_pid ".\n", thread_getpid(), thread->pid);

    uint16_t process_priority = thread->priority;

    irq_restore(irqstate);
    sched_switch(process_priority);
}

static void _wait(list_node_t *wait_list, unsigned irqstate)
{
    DEBUG("mbox: Thread %" PRIkernel_pid " _wait(): going blocked.\n",
          thread_getpid());

    thread_t *me = thread_get_active();

    sched_set_status(me, STATUS_MBOX_BLOCKED);
    thread_add_to_list(wait_list, me);
    irq_restore(irqstate);
    thread_yield();

    DEBUG("mbox: Thread %" PRIkernel_pid " _wait(): woke up.\n",
          thread_getpid());
}

int _mbox_put(mbox_t *mbox, msg_t *msg, int blocking)
{
    unsigned irqstate = irq_disable();

    list_node_t *next = list_remove_head(&mbox->readers);

    if (next) {
        DEBUG("mbox: Thread %" PRIkernel_pid " mbox 0x%08" PRIxPTR ": _tryput(): "
              "there's a waiter.\n", thread_getpid(), (uintptr_t)mbox);
        thread_t *thread =
            container_of((clist_node_t *)next, thread_t, rq_entry);
        *(msg_t *)thread->wait_data = *msg;
        _wake_waiter(thread, irqstate);
        return 1;
    }
    else {
        while (cib_full(&mbox->cib)) {
            if (blocking) {
                _wait(&mbox->writers, irqstate);
                irqstate = irq_disable();
            }
            else {
                irq_restore(irqstate);
                return 0;
            }
        }

        DEBUG("mbox: Thread %" PRIkernel_pid " mbox 0x%08" PRIxPTR ": _tryput(): "
              "queued message.\n", thread_getpid(), (uintptr_t)mbox);
        msg->sender_pid = thread_getpid();
        /* copy msg into queue */
        mbox->msg_array[cib_put_unsafe(&mbox->cib)] = *msg;
        irq_restore(irqstate);
        return 1;
    }
}

int _mbox_get(mbox_t *mbox, msg_t *msg, int blocking)
{
    unsigned irqstate = irq_disable();

    if (cib_avail(&mbox->cib)) {
        DEBUG("mbox: Thread %" PRIkernel_pid " mbox 0x%08" PRIxPTR ": _tryget(): "
              "got queued message.\n", thread_getpid(), (uintptr_t)mbox);
        /* copy msg from queue */
        *msg = mbox->msg_array[cib_get_unsafe(&mbox->cib)];
        list_node_t *next = list_remove_head(&mbox->writers);
        if (next) {
            thread_t *thread = container_of((clist_node_t *)next, thread_t,
                                            rq_entry);
            _wake_waiter(thread, irqstate);
        }
        else {
            irq_restore(irqstate);
        }
        return 1;
    }
    else if (blocking) {
        thread_get_active()->wait_data = msg;
        _wait(&mbox->readers, irqstate);
        /* sender has copied message */
        return 1;
    }
    else {
        irq_restore(irqstate);
        return 0;
    }
}
