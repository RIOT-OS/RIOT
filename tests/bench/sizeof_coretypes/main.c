/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Print size of core types
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cib.h"
#include "clist.h"
#include "panic.h"
#include "sched.h"
#include "list.h"
#include "mbox.h"
#include "msg.h"
#include "mutex.h"
#include "priority_queue.h"
#include "ringbuffer.h"
#include "rmutex.h"
#ifdef MODULE_CORE_THREAD_FLAGS
#include "thread_flags.h"
#endif
#include "thread.h"

#define P(NAME) printf("    tcb->%-11s            %3" PRIuSIZE "     %3" PRIuSIZE "\n", #NAME, \
                       sizeof(((thread_t *) 0)->NAME), \
                       offsetof(thread_t, NAME))

int main(void)
{
    puts("Sizeof RIOT core types\n");

    puts("                                size");

    printf("sizeof(cib_t):                  %3" PRIuSIZE "\n",
           sizeof(cib_t));
    printf("sizeof(clist_node_t):           %3" PRIuSIZE "\n",
           sizeof(clist_node_t));
    printf("sizeof(core_panic_t):           %3" PRIuSIZE "\n",
           sizeof(core_panic_t));
    printf("sizeof(kernel_pid_t):           %3" PRIuSIZE "\n",
           sizeof(kernel_pid_t));
    printf("sizeof(list_node_t):            %3" PRIuSIZE "\n",
           sizeof(list_node_t));
    printf("sizeof(mbox_t):                 %3" PRIuSIZE "\n",
           sizeof(mbox_t));
#ifdef MODULE_CORE_MSG
    printf("sizeof(msg_t):                  %3" PRIuSIZE "\n",
           sizeof(msg_t));
#else
    puts("sizeof(msg_t):                    0   (not enabled)");
#endif
    printf("sizeof(mutex_t):                %3" PRIuSIZE "\n",
           sizeof(mutex_t));
    printf("sizeof(priority_queue_node_t):  %3" PRIuSIZE "\n",
           sizeof(priority_queue_node_t));
    printf("sizeof(priority_queue_t):       %3" PRIuSIZE "\n",
           sizeof(priority_queue_t));
    printf("sizeof(ringbuffer_t):           %3" PRIuSIZE "\n",
           sizeof(ringbuffer_t));
    printf("sizeof(rmutex_t):               %3" PRIuSIZE "\n",
           sizeof(rmutex_t));
#ifdef MODULE_CORE_THREAD_FLAGS
    printf("sizeof(thread_flags_t):         %3" PRIuSIZE "\n",
           sizeof(thread_flags_t));
#else
    puts("sizeof(thread_flags_t):           0   (not enabled)");
#endif
    printf("\nTCB (thread_t) details:         size  offset\n");
    printf("sizeof(thread_t):               %3" PRIuSIZE "       -\n",
           sizeof(thread_t));
    P(sp);
    P(status);
    P(priority);
    P(pid);
#ifdef MODULE_CORE_THREAD_FLAGS
    P(flags);
#endif
    P(rq_entry);
#if defined(MODULE_CORE_MSG) || defined(MODULE_CORE_THREAD_FLAGS) || defined(MODULE_CORE_MBOX)
    P(wait_data);
#endif
#ifdef MODULE_CORE_MSG
    P(msg_waiters);
    P(msg_queue);
    P(msg_array);
#endif
#if defined(DEVELHELP) || IS_ACTIVE(SCHED_TEST_STACK) || defined(MODULE_MPU_STACK_GUARD)
    P(stack_start);
#endif
#ifdef DEVELHELP
    P(name);
    P(stack_size);
#endif

    puts("\n[SUCCESS]");
    return 0;
}
