/*
 * SPDX-FileCopyrightText: 2016 Loci Controls Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test application for the mpu_stack_guard pseudo-module
 *
 * @author Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "thread.h"
#include "mpu.h"

/* RIOT's MPU headers gracefully fail when no MPU is present.
 * Use this to catch if RIOT's features are correctly gating MPU use.
 */
#if !__MPU_PRESENT
#error "(!__MPU_PRESENT)"
#endif

#define CANARY_VALUE 0xdeadbeef

static struct {
    unsigned int canary;
    char stack[THREAD_STACKSIZE_MAIN];
} buf;

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
    printf("counter =%4d, SP = 0x%08x, canary = 0x%08x\n", counter, (unsigned int)__get_PSP(), buf.canary);

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
    puts("\nMPU Stack Guard Test\n");

    puts("If the test fails, the canary value will change unexpectedly");
    puts("after ~150 iterations. If the test succeeds, the MEM MANAGE HANDLER");
    puts("will trigger a RIOT kernel panic before the canary value changes.\n");

#ifdef MODULE_MPU_STACK_GUARD
    puts("The mpu_stack_guard module is present. Expect the test to succeed.\n");
#else
    puts("The mpu_stack_guard module is missing! Expect the test to fail.\n");
#endif

    buf.canary = CANARY_VALUE;
    thread_create(buf.stack, sizeof(buf.stack), THREAD_PRIORITY_MAIN - 1, 0, thread, NULL, "thread");

    return 0;
}
