/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       xtimer re-set test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"
extern void xtimer_dump_all(void);
int main(void)
{
    printf("This test tests re-setting of an already active timer.\n");
    printf("It should print three times \"now=<value>\", with values"
           " approximately 100ms (100000us) apart.\n");

    xtimer_t xtimer;
    xtimer_t xtimer2;

    kernel_pid_t me = thread_getpid();

    xtimer_set_wakeup(&xtimer, ((uint32_t)1<<31), me);
    xtimer_set_wakeup(&xtimer2, ((uint32_t)1<<31), me);

    xtimer_set_wakeup(&xtimer, ((uint32_t)1<<16), me);
    xtimer_set_wakeup(&xtimer2, ((uint32_t)1<<16), me);

    xtimer_set_wakeup(&xtimer, 4000000, me);
    xtimer_set_wakeup(&xtimer2, 3000000, me);

    xtimer_set_wakeup(&xtimer, 200000, me);
    xtimer_set_wakeup(&xtimer2, 100000, me);

    printf("now=%u\n", (unsigned)xtimer_now());
    thread_sleep();
    printf("now=%u\n", (unsigned)xtimer_now());
    thread_sleep();
    printf("now=%u\n", (unsigned)xtimer_now());

    printf("Test completed!\n");

    return 0;
}
