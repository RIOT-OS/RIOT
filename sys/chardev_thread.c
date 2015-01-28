/**
 * Character device messaging loop.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup chardev
 * @{
 * @file    chardev_thread.c
 * @brief   Runs an infinite loop in a separate thread to handle access to character devices such as uart.
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>
#include <errno.h>

#include "irq.h"
#include "msg.h"

#include "ringbuffer.h"
#include "posix_io.h"

/* increase stack size in uart0 when setting this to 1 */
#define ENABLE_DEBUG    (0)
#include "debug.h"

static int min(int a, int b)
{
    if (b > a) {
        return a;
    }
    else {
        return b;
    }
}

void chardev_loop(ringbuffer_t *rb)
{
    msg_t m;

    kernel_pid_t reader_pid = KERNEL_PID_UNDEF;
    struct posix_iop_t *r = NULL;

    puts("UART0 thread started.");

    while (1) {
        msg_receive(&m);

        if (!msg_sent_by_int(&m)) {
            DEBUG("Receiving message from another thread: ");

            switch(m.type) {
                case OPEN:
                    DEBUG("OPEN\n");
                    if (reader_pid == KERNEL_PID_UNDEF) {
                        reader_pid = m.sender_pid;
                        /* no error */
                        m.content.value = 0;
                    }
                    else {
                        m.content.value = -EBUSY;
                    }

                    msg_reply(&m, &m);
                    break;

                case READ:
                    DEBUG("READ\n");
                    if (m.sender_pid != reader_pid) {
                        m.content.value = -EINVAL;
                        r = NULL;
                        msg_reply(&m, &m);
                    }
                    else {
                        r = (struct posix_iop_t *)(void*)m.content.ptr;
                    }

                    break;

                case CLOSE:
                    DEBUG("CLOSE\n");
                    if (m.sender_pid == reader_pid) {
                        DEBUG("uart0_thread: closing file from %" PRIkernel_pid "\n", reader_pid);
                        reader_pid = KERNEL_PID_UNDEF;
                        r = NULL;
                        m.content.value = 0;
                    }
                    else {
                        m.content.value = -EINVAL;
                    }

                    msg_reply(&m, &m);
                    break;

                default:
                    DEBUG("UNKNOWN\n");
                    m.content.value = -EINVAL;
                    msg_reply(&m, &m);
            }
        }

        if (rb->avail && (r != NULL)) {
            DEBUG("Data is available\n");
            unsigned state = disableIRQ();
            int nbytes = min(r->nbytes, rb->avail);
            DEBUG("uart0_thread [%i]: sending %i bytes received from %" PRIkernel_pid " to pid %" PRIkernel_pid "\n", thread_getpid(), nbytes, m.sender_pid, reader_pid);
            ringbuffer_get(rb, r->buffer, nbytes);
            r->nbytes = nbytes;

            m.sender_pid = reader_pid;
            m.type = OPEN;
            m.content.ptr = (char *)r;

            msg_reply(&m, &m);

            r = NULL;
            restoreIRQ(state);
        }
    }
}

void *chardev_thread_entry(void *rb_)
{
    ringbuffer_t *rb = rb_;
    chardev_loop(rb);
    return NULL;
}
