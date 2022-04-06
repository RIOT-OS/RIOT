/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Prints stack usage in JSON metric format, when a thread ends
 *
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "thread.h"
#include "log.h"

#if MODULE_FMT
#include "fmt.h"
#endif

#if MODULE_FMT
/* fmt's `print_str()` needs very little stack. ~200 total was fine on Cortex-M. */
# define MIN_SIZE   (THREAD_STACKSIZE_TINY)
#else
# define MIN_SIZE   (THREAD_STACKSIZE_TINY + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

void print_stack_usage_metric(const char *name, void *stack, unsigned max_size)
{
    unsigned free = thread_measure_stack_free(stack);

    if ((LOG_LEVEL >= LOG_INFO) &&
        (thread_get_stacksize(thread_get_active()) >= MIN_SIZE)) {
#if MODULE_FMT
        print_str("{ \"threads\": [{ \"name\": \"");
        print_str(name);
        print_str(", \"stack_size\": ");
        print_u32_dec(max_size);
        print_str(", \"stack_used\": ");
        print_u32_dec(max_size - free);
        print_str("}]}\n");
#else
        printf(
            "{ \"threads\": [{ \"name\": \"%s\", \"stack_size\": %u, \"stack_used\": %u }]}\n",
            name, max_size, max_size - free);
#endif
    }
}

#ifdef DEVELHELP
void test_utils_print_stack_usage(void)
{
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = (thread_t *)sched_threads[i];

        if (p == NULL) {
            continue;
        }
        print_stack_usage_metric(p->name, p->stack_start, p->stack_size);
    }
}
#endif
