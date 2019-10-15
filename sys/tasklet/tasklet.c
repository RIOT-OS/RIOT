/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <inttypes.h>
#include <errno.h>

#include "tasklet.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Stack for the interrupt event handler thread */
static char _tasklet_stack[THREAD_STACKSIZE_DEFAULT];

/* PID of the interrupt handler thread, KERNEL_PID_UNDEF if not created yet */
static kernel_pid_t _tasklet_thread = KERNEL_PID_UNDEF;

/* Interrupt event queue */
static event_queue_t tasklet_queue = EVENT_QUEUE_INIT_DETACHED;

static void _tasklet_handler(event_t *event)
{
    tasklet_t *tasklet = (tasklet_t *)event;
    assert(tasklet != NULL);

    /* handle the pending interrupt */
    tasklet->pending = false;
    tasklet->task(tasklet->ctx);
}

static void *_tasklet_loop(void *arg)
{
    (void)arg;

    DEBUG("[%s] starts\n", __func__);

    /* bind the queue */
    event_queue_claim(&tasklet_queue);

    /* doesn't return */
    event_loop(&tasklet_queue);

    return NULL;
}

int tasklet_schedule(tasklet_t * tasklet)
{
    assert(tasklet != NULL);
    assert(tasklet->task != NULL);

    DEBUG("[%s] tasklet %p\n", __func__, (void *)tasklet);

    if (tasklet->pending) {
        DEBUG("[%s] interrupt event %p is already pending\n",
              __func__, (void *)tasklet);
        return -EALREADY;
    }

    /* disable interrupts */
    unsigned state = irq_disable();

    /* create the handler thread if not created yet */
    if (_tasklet_thread == KERNEL_PID_UNDEF) {
        DEBUG("[%s] create tasklet thread\n", __func__);
        _tasklet_thread = thread_create(_tasklet_stack,
                                            sizeof(_tasklet_stack),
                                            TASKLET_HANDLER_PRIO,
                                            THREAD_CREATE_WOUT_YIELD |
                                            THREAD_CREATE_STACKTEST,
                                            _tasklet_loop, NULL, "irq_handler");
        assert(_tasklet_thread != KERNEL_PID_UNDEF);

        /* intialize the queue unbind */
        event_queue_init_detached(&tasklet_queue);
    }

    /* initialize the interrupt event */
    tasklet->event.handler = _tasklet_handler;
    tasklet->pending = true;

    /* restore previous interrupt state */
    irq_restore(state);

    /* queue the interrupt event */
    event_post(&tasklet_queue, (event_t *)tasklet);

    return 0;
}
