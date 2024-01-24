/*
 * Copyright (C) 2016 Loci Controls Inc.
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
 * @brief Test application for the pmp_stack_guard pseudo-module
 *
 * @author Ian Martin <ian@locicontrols.com>
 * @author Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "thread.h"
#include "thread_arch.h"
#include "pmp.h"

#define CANARY_VALUE 0xdeadbeef

static struct {
    char overflow_buffer[128];
    unsigned int canary;
    char stack[THREAD_STACKSIZE_MAIN];
} buf;


static inline unsigned int __get_SP(void) {
    unsigned int __tmp;
    __asm__ volatile ("mv %0, sp" : "=r"(__tmp));
    return __tmp;
}

/* Tell modern GCC (12.x) to not complain that this infinite recursion is
 * bound to overflow the stack - this is exactly what this test wants to do :)
 *
 * Also, tell older versions of GCC that do not know about -Winfinit-recursion
 * that it is safe to ignore `GCC diagnostics ignored "-Winfinit-recursion"`.
 * They behave as intended in this case :)
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Winfinite-recursion"
static int recurse(int counter)
{
    printf("counter =%4d, SP = 0x%08x, canary = 0x%08x\n", counter, (unsigned int)__get_SP(), buf.canary);

    if (buf.canary != CANARY_VALUE) {
        printf("canary = 0x%08x\nTest failed.\n", buf.canary);

        for (;;) {
            thread_sleep();
        }
    }

    counter++;

    /* Recursing twice here prevents the compiler from optimizing-out the recursion. */
    return recurse(counter) + recurse(counter);
}
#pragma GCC diagnostic pop

static void *thread(void *arg)
{
    (void) arg;

    recurse(0);

    return NULL;
}

int main(void)
{
    puts("\nPMP Stack Guard Test\n");
    puts("If the test fails, the canary value will change unexpectedly");
    puts("after ~50 iterations. If the test succeeds, the MEM MANAGE HANDLER");
    puts("will trigger a RIOT kernel panic before the canary value changes.\n");
#ifdef MODULE_PMP_STACK_GUARD
    puts("The pmp_stack_guard module is present. Expect the test to succeed.\n");
#else
    puts("The pmp_stack_guard module is missing! Expect the test to fail.\n");
#endif

    buf.canary = CANARY_VALUE;
    thread_create(buf.stack, sizeof(buf.stack), THREAD_PRIORITY_MAIN - 1, 0, thread, NULL, "thread");

    return 0;
}
