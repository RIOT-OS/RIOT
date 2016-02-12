/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>

#include "sched.h"
#include "thread.h"

void thread_print_msg_queue(void)
{
    volatile tcb_t  *tcb        = sched_active_thread;
    volatile cib_t  *msg_queue  = &tcb->msg_queue;
    msg_t           *msg_array  = tcb->msg_array;
    unsigned int    i           = msg_queue->read_count & msg_queue->mask;

    printf("Message queue of thread %" PRIkernel_pid "\n", tcb->pid);
    printf("    size: %u (avail: %d)\n", msg_queue->mask + 1,
           cib_avail((cib_t *)msg_queue));

    for (; i != (msg_queue->write_count & msg_queue->mask);
         i = (i + 1) & msg_queue->mask) {
        msg_t *m = &msg_array[i];
        printf("    * %u: sender: %" PRIkernel_pid ", type: 0x%04" PRIu16
               ", content: %" PRIu32 " (%p)\n", i, m->sender_pid, m->type,
               m->content.value, (void *)m->content.ptr);
    }
}

/** @} */
