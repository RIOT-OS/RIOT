/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     sys_pipe
 * @{
 * @file
 * @brief       Implementation for statically allocated pipes.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include "irq.h"
#include "pipe.h"
#include "sched.h"

typedef unsigned (*ringbuffer_op_t)(ringbuffer_t *restrict rb, char *buf, unsigned n);

static ssize_t pipe_rw(ringbuffer_t *rb,
                       void *buf,
                       size_t n,
                       thread_t **other_op_blocked,
                       thread_t **this_op_blocked,
                       ringbuffer_op_t ringbuffer_op)
{
    if (n == 0) {
        return 0;
    }

    while (1) {
        unsigned old_state = irq_disable();

        unsigned count = ringbuffer_op(rb, buf, n);

        if (count > 0) {
            thread_t *other_thread = *other_op_blocked;
            int other_prio = -1;
            if (other_thread) {
                *other_op_blocked = NULL;
                other_prio = other_thread->priority;
                sched_set_status(other_thread, STATUS_PENDING);
            }

            irq_restore(old_state);

            if (other_prio >= 0) {
                sched_switch(other_prio);
            }

            return count;
        }
        else if (*this_op_blocked || irq_is_in()) {
            irq_restore(old_state);
            return 0;
        }
        else {
            *this_op_blocked = thread_get_active();

            sched_set_status(thread_get_active(), STATUS_SLEEPING);
            irq_restore(old_state);
            thread_yield_higher();
        }
    }
}

ssize_t pipe_read(pipe_t *pipe, void *buf, size_t n)
{
    return pipe_rw(pipe->rb, (char *) buf, n,
                   &pipe->write_blocked, &pipe->read_blocked, ringbuffer_get);
}

ssize_t pipe_write(pipe_t *pipe, const void *buf, size_t n)
{
    return pipe_rw(pipe->rb, (char *) buf, n,
                   &pipe->read_blocked, &pipe->write_blocked, (ringbuffer_op_t) ringbuffer_add);
}

void pipe_init(pipe_t *pipe, ringbuffer_t *rb, void (*free)(void *))
{
    *pipe = (pipe_t) {
        .rb = rb,
        .read_blocked = NULL,
        .write_blocked = NULL,
        .free = free,
    };
}
