/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <inttypes.h>
#include <errno.h>

#include "irq_handler.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Stack for the interrupt event handler thread */
static char _irq_handler_stack[THREAD_STACKSIZE_DEFAULT];

/* PID of the interrupt handler thread, KERNEL_PID_UNDEF if not created yet */
static kernel_pid_t _irq_handler_thread = KERNEL_PID_UNDEF;

/* Interrupt event queue */
static event_queue_t irq_queue = EVENT_QUEUE_INIT_DETACHED;

static void _irq_handler(event_t *event)
{
    irq_event_t *irq = (irq_event_t *)event;
    assert(irq != NULL);

    /* handle the pending interrupt */
    irq->pending = false;
    irq->isr(irq->ctx);
}

static void *_irq_loop(void *arg)
{
    (void)arg;

    DEBUG("[%s] starts\n", __func__);

    /* bind the queue */
    event_queue_claim(&irq_queue);

    /* doesn't return */
    event_loop(&irq_queue);

    return NULL;
}

int irq_event_add(irq_event_t * irq)
{
    assert(irq != NULL);
    assert(irq->isr != NULL);

    DEBUG("[%s] irq %p\n", __func__, (void *)irq);

    if (irq->pending) {
        DEBUG("[%s] interrupt event %p is already pending\n",
              __func__, (void *)irq);
        return -EALREADY;
    }

    /* disable interrupts */
    unsigned state = irq_disable();

    /* create the handler thread if not created yet */
    if (_irq_handler_thread == KERNEL_PID_UNDEF) {
        DEBUG("[%s] create irq_handler thread\n", __func__);
        _irq_handler_thread = thread_create(_irq_handler_stack,
                                            sizeof(_irq_handler_stack),
                                            IRQ_HANDLER_PRIO,
                                            THREAD_CREATE_WOUT_YIELD |
                                            THREAD_CREATE_STACKTEST,
                                            _irq_loop, NULL, "irq_handler");
        assert(_irq_handler_thread != KERNEL_PID_UNDEF);

        /* initialize the queue unbind */
        event_queue_init_detached(&irq_queue);
    }

    /* initialize the interrupt event */
    irq->event.handler = _irq_handler;
    irq->pending = true;

    /* restore previous interrupt state */
    irq_restore(state);

    /* queue the interrupt event */
    event_post(&irq_queue, (event_t *)irq);

    return 0;
}
