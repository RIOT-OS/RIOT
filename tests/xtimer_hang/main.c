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
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "thread.h"
#include "log.h"

#if defined(MAIN_THREAD_PIN) || defined(WORKER_THREAD_PIN_1)
#include "board.h"
#include "periph/gpio.h"

typedef struct {
    uint32_t time;
    gpio_t pin;
} timer_arg_t;
#endif

#define TEST_TIME_S             (10LU)
#define TEST_INTERVAL_MS        (100LU)
#define TEST_TIMER_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)

#define TEST_SLEEP_TIME_1 (1000)
#define TEST_SLEEP_TIME_2 (1100)

char stack_timer1[TEST_TIMER_STACKSIZE];
char stack_timer2[TEST_TIMER_STACKSIZE];

void *timer_func(void *arg)
{
    LOG_DEBUG("run thread %" PRIkernel_pid "\n", thread_getpid());

#if defined(WORKER_THREAD_PIN_1) && defined(WORKER_THREAD_PIN_2)
    gpio_init((*(timer_arg_t *)(arg)).pin, GPIO_OUT);
#endif

    while (1) {
#if defined(WORKER_THREAD_PIN_1) && defined(WORKER_THREAD_PIN_2)
        gpio_set((*(timer_arg_t *)(arg)).pin);
        gpio_clear((*(timer_arg_t *)(arg)).pin);
#endif
        xtimer_usleep(*(uint32_t *)(arg));
    }
}

int main(void)
{
#if defined(MAIN_THREAD_PIN)
    gpio_t main_pin = MAIN_THREAD_PIN;
    gpio_init(main_pin, GPIO_OUT);
#endif

#if defined(WORKER_THREAD_PIN_1) && defined(WORKER_THREAD_PIN_2)
    timer_arg_t sleep_timer1 = { TEST_SLEEP_TIME_1, WORKER_THREAD_PIN_1 };
    timer_arg_t sleep_timer2 = { TEST_SLEEP_TIME_2, WORKER_THREAD_PIN_2 };
#else
    uint32_t sleep_timer1 = TEST_SLEEP_TIME_1;
    uint32_t sleep_timer2 = TEST_SLEEP_TIME_2;
#endif
    LOG_DEBUG("[INIT]\n");

    thread_create(stack_timer1, TEST_TIMER_STACKSIZE,
                  2, THREAD_CREATE_STACKTEST,
                  timer_func, &sleep_timer1, "timer1");
    thread_create(stack_timer2, TEST_TIMER_STACKSIZE,
                  3, THREAD_CREATE_STACKTEST,
                  timer_func, &sleep_timer2, "timer2");

    uint32_t now = 0;
    uint32_t start = xtimer_now_usec();
    uint32_t until = start + (TEST_TIME_S * US_PER_SEC);
    uint32_t interval = TEST_INTERVAL_MS * US_PER_MS;
    xtimer_ticks32_t last_wakeup = xtimer_now();

    puts("[START]");
    while ((now = xtimer_now_usec()) < until) {
        unsigned percent = (100 * (now - start)) / (until - start);
#if defined(MAIN_THREAD_PIN)
        gpio_set(main_pin);
#endif
        xtimer_periodic_wakeup(&last_wakeup, interval);
#if defined(MAIN_THREAD_PIN)
        gpio_clear(main_pin);
#endif
        printf("Testing (%3u%%)\n", percent);
    }
    puts("Testing (100%)");
    puts("[SUCCESS]");
    return 0;
}
