    /*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Benchmark measuring the GPIO driver's run-time performance
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "periph/gpio.h"

#define RUNS            (1000 * 1000UL)

static void print_time(uint32_t time, const char *name)
{
    uint32_t full = (time / RUNS);
    uint32_t div  = (time - (full * RUNS)) / (RUNS / 1000);

    printf("%11s: %9luus  ---  %2u.%03uus per call\n", name,
           (unsigned long) time, (unsigned)full, (unsigned)div);
}

static void bench_pin(gpio_t pin)
{
    uint32_t time;

    /* gpio_set */
    time = xtimer_now_usec();
    for (unsigned long i = 0; i < RUNS; i++) {
        gpio_set(pin);
    }
    time = (xtimer_now_usec() - time);
    print_time(time, "gpio_set");

    /* gpio_clear */
    time = xtimer_now_usec();
    for (unsigned long i = 0; i < RUNS; i++) {
        gpio_clear(pin);
    }
    time = (xtimer_now_usec() - time);
    print_time(time, "gpio_clear");

    /* gpio_clear */
    time = xtimer_now_usec();
    for (unsigned long i = 0; i < RUNS; i++) {
        gpio_toggle(pin);
    }
    time = (xtimer_now_usec() - time);
    print_time(time, "gpio_toggle");

    /* gpio_clear */
    time = xtimer_now_usec();
    for (unsigned long i = 0; i < RUNS; i++) {
        gpio_write(pin, 1);
    }
    time = (xtimer_now_usec() - time);
    print_time(time, "gpio_write");

    /* gpio_read */
    time = xtimer_now_usec();
    for (unsigned long i = 0; i < RUNS; i++) {
        int tmp = gpio_read(pin);
        (void)tmp;
    }
    time = (xtimer_now_usec() - time);
    print_time(time, "gpio_read");

}

int main(void)
{
    puts("\nGPIO driver run-time performance benchmark\n");

    bench_pin(GPIO_PIN(0, 0));

    puts("\n --- DONE ---");

    return 0;
}
