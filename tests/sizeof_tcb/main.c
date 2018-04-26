/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief  Print the size of thread_t.
 *
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stddef.h>

#include "thread.h"

#define P(NAME) printf("\t%-11s%4u%4u\n", #NAME, \
                       (unsigned)sizeof(((thread_t *) 0)->NAME), \
                       (unsigned)offsetof(thread_t, NAME));

int main(void)
{
    puts("\tmember, sizeof, offsetof");

    printf("sizeof(thread_t): %u\n", (unsigned)sizeof(thread_t));

    P(sp);
    P(status);
    P(priority);
    P(pid);
#ifdef MODULE_CORE_THREAD_FLAGS
    P(flags);
#endif
    P(rq_entry);
#ifdef MODULE_CORE_MSG
    P(wait_data);
    P(msg_waiters);
    P(msg_queue);
    P(msg_array);
#endif
#ifdef DEVELHELP
    P(name);
#endif
#if defined(DEVELHELP) || defined(SCHED_TEST_STACK) || defined(MODULE_MPU_STACK_GUARD)
    P(stack_start);
#endif

#ifdef DEVELHELP
    P(stack_size);
#endif

    puts("SUCCESS");
    return 0;
}
