/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     test_utils_print_stack_usage
 * @{
 *
 * @file
 * @brief       Prints stack usage in JSON metric format, when a thread ends
 *
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdalign.h>
#include <stdint.h>

#include "thread.h"
#include "log.h"
#include "test_utils/print_stack_usage.h"

#if MODULE_FMT
#  include "fmt.h"
#else
#  include <stdio.h>
#endif

#if MODULE_FMT
#  define MIN_SIZE  (THREAD_STACKSIZE_TINY)
#else
#  define MIN_SIZE  (THREAD_STACKSIZE_TINY + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

void print_stack_usage_metric(const char *name, void *stack, unsigned max_size)
{
    /* the stack is only filled with canaries and the stack size and thread
     * names are only available with DEVELHELP */
    if (!IS_ACTIVE(DEVELHELP)) {
        return;
    }

    /* thread_create() aligns the start of an unaligned stack (e.g. the idle
     * stack passed by core/lib/init.c), so the stack painting starts there */
    unsigned misalignment = (uintptr_t)stack % alignof(void *);
    if (misalignment) {
        misalignment = alignof(void *) - misalignment;
    }
    unsigned free = measure_stack_free_internal((char *)stack + misalignment,
                                                max_size - misalignment);

    if ((LOG_LEVEL >= LOG_INFO) &&
        (thread_get_stacksize(thread_get_active()) >= MIN_SIZE)) {
#if MODULE_FMT
        print_str("{ \"threads\": [{ \"name\": \"");
        print_str(name);
        print_str("\", \"stack_size\": ");
        print_u32_dec(max_size);
        print_str(", \"stack_used\": ");
        print_u32_dec(max_size - free);
        print_str("}]}\n");
#else
        printf("{ \"threads\": [{ \"name\": \"%s\", \"stack_size\": %u, \"stack_used\": %u }]}\n",
               name, max_size, max_size - free);
#endif
    }
}
