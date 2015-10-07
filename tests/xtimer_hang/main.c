/*
 * Copyright (C) 2015 Daniel Krebs <github@daniel-krebs.net>
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       xtimer test application
 *
 * This test used to hang many boards.
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "thread.h"

#define TEST_SECONDS        (10U)
#define TEST_TIME           (TEST_SECONDS * SEC_IN_USEC)
#define STACKSIZE_TIMER     (1024)

char stack_timer1[STACKSIZE_TIMER];
char stack_timer2[STACKSIZE_TIMER];

long unsigned int count = 0;

void* timer_func1(void* arg)
{
    (void)arg;
    while(1)
    {
        xtimer_usleep(1000);
    }
}

void* timer_func2(void* arg)
{
    (void)arg;
    while(1)
    {
        xtimer_usleep(1100);
    }
}

int main(void)
{

    puts("xTimer hang test\n");

    printf("This test will print \"Testing... (<percentage>)\" every 100ms for %u seconds.\n", TEST_SECONDS);
    printf("If it stops before, the test failed.\n\n");

    thread_create(stack_timer1,
                  STACKSIZE_TIMER,
                  2,
                  CREATE_STACKTEST,
                  timer_func1,
                  NULL,
                  "timer1");

    thread_create(stack_timer2,
                  STACKSIZE_TIMER,
                  3,
                  CREATE_STACKTEST,
                  timer_func2,
                  NULL,
                  "timer2");

    uint32_t end = xtimer_now() + TEST_TIME;
    while(xtimer_now() < end)
    {
        count++;
        xtimer_usleep(100U * 1000);
        printf("Testing... (%lu%%)\n", count);
    }

    printf("Test successful.\n");

    return 0;
}
