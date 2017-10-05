/*
 * Copyright (C) 2015 Daniel Krebs <github@daniel-krebs.net>
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 HAW Hamburg
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
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "thread.h"
#include "log.h"

#define TEST_TIME_S             (10LU)
#define TEST_INTERVAL_MS        (100LU)
#define TEST_TIMER_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)

char stack_timer1[TEST_TIMER_STACKSIZE];
char stack_timer2[TEST_TIMER_STACKSIZE];

void* timer_func(void* arg)
{
    LOG_DEBUG("run thread %" PRIkernel_pid "\n", thread_getpid());
    while(1) {
        xtimer_usleep(*(uint32_t *)(arg));
    }
}

int main(void)
{
    LOG_DEBUG("[INIT]\n");
    uint32_t sleep_timer1 = 1000;
    uint32_t sleep_timer2 = 1100;

    thread_create(stack_timer1, TEST_TIMER_STACKSIZE,
                  2, THREAD_CREATE_STACKTEST,
                  timer_func, &sleep_timer1, "timer1");
    thread_create(stack_timer2, TEST_TIMER_STACKSIZE,
                  3, THREAD_CREATE_STACKTEST,
                  timer_func, &sleep_timer2, "timer2");

    uint32_t now = 0;
    uint32_t start = xtimer_now_usec();
    uint32_t until = start + (TEST_TIME_S * US_PER_SEC);
    puts("[START]");
    while((now = xtimer_now_usec()) < until) {
        unsigned percent = (100 * (now - start)) / (until - start);
        xtimer_usleep(TEST_INTERVAL_MS * US_PER_MS);
        printf("Testing (%3u%%)\n", percent);
    }
    puts("Testing (100%)");
    puts("[SUCCESS]");
    return 0;
}
