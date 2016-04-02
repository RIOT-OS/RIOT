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

#define P(NAME) printf("\t%-*s%4zu%4zu\n", 11, #NAME, sizeof(((thread_t *) 0)->NAME), offsetof(thread_t, NAME));

int main(void)
{
    puts("\tmember, sizeof, offsetof");

    printf("sizeof(thread_t): %zu\n", sizeof(thread_t));

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
#if defined(DEVELHELP) || defined(SCHED_TEST_STACK)
    P(stack_start);
#endif

#ifdef DEVELHELP
    P(stack_size);
#endif

    puts("Done.");
    return 0;
}
