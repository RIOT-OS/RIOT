/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 * @{
 *
 * @file
 * @brief       Kernel messaging bus implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stddef.h>
#include <inttypes.h>
#include <assert.h>
#include "sched.h"
#include "msg.h"
#include "irq.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

struct msg_bus_consumer {
    msg_t *m;
    const uint16_t *type;
    uint8_t max_idx;
};

int msg_bus_post(uint16_t type, void *arg)
{
    const bool in_irq = irq_is_in();
    int threads_woken = 0;

    msg_t m = {
        .sender_pid = in_irq ? KERNEL_PID_ISR : sched_active_pid,
        .type = type,
        .content.ptr  = arg,
    };

    unsigned state = irq_disable();

    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        struct msg_bus_consumer *consumer;
        thread_t *p = (thread_t *)sched_threads[i];

        if (p == NULL) {
            continue;
        }

        if (p->status != STATUS_RECEIVE_BUS_BLOCKED) {
            continue;
        }

        assert(p->wait_data != NULL);

        consumer = p->wait_data;
        for (int j = consumer->max_idx; j >= 0; --j) {

            if (consumer->type[j] != type) {
                continue;
            }

            /* copy msg to target */
            *consumer->m = m;
            sched_set_status(p, STATUS_PENDING);
            ++threads_woken;
            break;
        }
    }

    irq_restore(state);

    /* no need to run scheduler if no threads waited for an event */
    if (threads_woken == 0) {
        return 0;
    }

    if (in_irq) {
        sched_context_switch_request = 1;
    } else {
        thread_yield_higher();
    }

    return threads_woken;
}

void msg_bus_receive(msg_t *m, const uint16_t *events, uint8_t num_events)
{
    assert(m);
    assert(events);
    assert(num_events > 0);

    unsigned state = irq_disable();

    struct msg_bus_consumer consumer = {
        .m = m,
        .type = events,
        .max_idx = num_events - 1,
    };

    thread_t *me = (thread_t *)sched_threads[sched_active_pid];
    me->wait_data = &consumer;
    sched_set_status(me, STATUS_RECEIVE_BUS_BLOCKED);

    irq_restore(state);
    thread_yield_higher();

    /* sender copied message */
    assert(sched_active_thread->status != STATUS_RECEIVE_BUS_BLOCKED);
}
