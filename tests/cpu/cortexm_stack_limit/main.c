/*
 * SPDX-FileCopyrightText: 2024 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test application for the cortexm_stack_limit pseudo-module
 *
 * @author Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "thread.h"
#include "board.h"

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
    if (buf.canary != CANARY_VALUE) {
#ifdef LED0_ON
        LED0_ON;
#endif
#ifdef LED1_ON
        LED1_ON;
#endif
#ifdef LED2_ON
        LED2_ON;
#endif
#ifdef LED3_ON
        LED3_ON;
#endif

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
    puts("\nCortex-M Stack limit test\n");

    puts("If the test fails, all onboard LEDs will be on");

#ifdef MODULE_CORTEXM_STACK_LIMIT
    puts("The cortexm_stack_limit module is present. Expect the board to crash"\
         " without onboard LEDs on.\n");
#else
    puts("The cortexm_stack_limit module is missing! Expect the test to crash"\
         " with all onboard LEDs on\n");
#endif

    buf.canary = CANARY_VALUE;
    thread_create(buf.stack, sizeof(buf.stack), THREAD_PRIORITY_MAIN + 1,
                  0, thread, NULL, "thread");

    return 0;
}
