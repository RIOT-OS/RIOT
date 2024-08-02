/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for the Peripheral GPIO Low-Level API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "periph/gpio.h"
#include "periph/gpio_ll.h"
#include "test_utils/expect.h"
#include "time_units.h"
#include "ztimer.h"

#ifndef COMPENSATE_OVERHEAD
#define COMPENSATE_OVERHEAD 1
#endif

#ifndef PORT_OUT
#  if defined(GPIO_PORT_0)
#    define PORT_OUT        GPIO_PORT_0
#    define PORT_OUT_NUM    0
#  elif defined(GPIO_PORT_1)
#    define PORT_OUT        GPIO_PORT_1
#    define PORT_OUT_NUM    1
#  elif defined(GPIO_PORT_2)
#    define PORT_OUT        GPIO_PORT_2
#    define PORT_OUT_NUM    2
#  endif
#endif

static gpio_port_t port_out = PORT_OUT;

static void print_summary_compensated(uint_fast16_t loops, uint32_t duration,
                                      uint32_t duration_uncompensated)
{
    printf("%" PRIuFAST16 " iterations took %" PRIu32 " us "
           "(%" PRIu32 " us uncompensated)\n",
           loops, duration, duration_uncompensated);
    printf("Two square waves pins at %12" PRIu32 " Hz "
           "(%12" PRIu32 " Hz uncompensated)\n",
           (uint32_t)((uint64_t)US_PER_SEC * loops / duration),
           (uint32_t)((uint64_t)US_PER_SEC * loops / duration_uncompensated));
#ifdef CLOCK_CORECLOCK
    uint64_t divisor = (uint64_t)US_PER_SEC * loops / CLOCK_CORECLOCK;
    uint32_t cycles = (duration + divisor / 2) / divisor;
    uint32_t cycles_uncompensated = (duration_uncompensated + divisor / 2)
                                  / divisor;
    printf("~%" PRIu32 " CPU cycles per square wave period "
           "(~%" PRIu32 " cycles uncompensated)\n",
           cycles, cycles_uncompensated);
    if (cycles <= 2) {
        puts(":-D");
    }
    else if (cycles <= 4) {
        puts(":-)");
    }
    else if (cycles <= 8) {
        puts(":-|");
    }
    else if (cycles <= 16) {
        puts(":-(");
    }
    else {
        puts(":'-(");
    }
#endif
}

static void print_summary_uncompensated(uint_fast16_t loops, uint32_t duration)
{
    printf("%" PRIuFAST16 " iterations took %" PRIu32 " us\n",
           loops, duration);
    printf("Two square waves pins at %12" PRIu32 " Hz\n",
           (uint32_t)((uint64_t)US_PER_SEC * loops / duration));
#ifdef CLOCK_CORECLOCK
    uint64_t divisor = (uint64_t)US_PER_SEC * loops / CLOCK_CORECLOCK;
    uint32_t cycles = (duration + divisor / 2) / divisor;
    printf("~%" PRIu32 " CPU cycles per square wave period\n", cycles);
    if (cycles <= 2) {
        puts(":-D");
    }
    else if (cycles <= 4) {
        puts(":-)");
    }
    else if (cycles <= 8) {
        puts(":-|");
    }
    else if (cycles <= 16) {
        puts(":-(");
    }
    else {
        puts(":'-(");
    }
#endif
}

int main(void)
{
    static const uint_fast16_t loops = 50000;
    uint32_t loop_overhead = 0;

    uword_t mask_both = (1U << PIN_OUT_0) | (1U << PIN_OUT_1);

    puts("\n"
         "Benchmarking GPIO APIs\n"
         "======================");

    if (COMPENSATE_OVERHEAD) {
        puts("\n"
             "estimating loop overhead for compensation\n"
             "-----------------------------------------");
        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            __asm__ volatile ("" : : : );
        }
        loop_overhead = ztimer_now(ZTIMER_USEC) - start;
        printf("%" PRIu32 " us for %" PRIuFAST16 " iterations\n",
               loop_overhead, loops);
    }

    {
        puts("\n"
             "periph/gpio: Using 2x gpio_set() and 2x gpio_clear()\n"
             "---------------------------------------------------");
        gpio_t p0 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_0);
        gpio_t p1 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_1);
        gpio_init(p0, GPIO_OUT);
        gpio_init(p1, GPIO_OUT);

        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_set(p0);
            gpio_set(p1);
            gpio_clear(p0);
            gpio_clear(p1);
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    {
        puts("\n"
             "periph/gpio_ll: Using gpio_ll_set() and gpio_ll_clear()\n"
             "-------------------------------------------------------");
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
        };
        expect(0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
        expect(0 == gpio_ll_init(port_out, PIN_OUT_1, conf));

        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_ll_set(port_out, (1UL << PIN_OUT_0) | (1UL << PIN_OUT_1));
            gpio_ll_clear(port_out, (1UL << PIN_OUT_0) | (1UL << PIN_OUT_1));
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    {
        puts("\n"
             "periph/gpio: Using 4x gpio_toggle()\n"
             "-----------------------------------");
        gpio_t p0 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_0);
        gpio_t p1 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_1);
        gpio_init(p0, GPIO_OUT);
        gpio_init(p1, GPIO_OUT);

        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_toggle(p0);
            gpio_toggle(p1);
            gpio_toggle(p0);
            gpio_toggle(p1);
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    {
        puts("\n"
             "periph/gpio_ll: Using 2x gpio_ll_toggle()\n"
             "-----------------------------------------");
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
        };
        expect(0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
        expect(0 == gpio_ll_init(port_out, PIN_OUT_1, conf));

        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_ll_toggle(port_out, mask_both);
            gpio_ll_toggle(port_out, mask_both);
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    {
        puts("\n"
             "periph/gpio: Using 4x gpio_write()\n"
             "----------------------------------");
        gpio_t p0 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_0);
        gpio_t p1 = GPIO_PIN(PORT_OUT_NUM, PIN_OUT_1);
        gpio_init(p0, GPIO_OUT);
        gpio_init(p1, GPIO_OUT);

        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_write(p0, 1);
            gpio_write(p1, 1);
            gpio_write(p0, 0);
            gpio_write(p1, 0);
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    {
        puts("\n"
             "periph/gpio_ll: Using 2x gpio_ll_write()\n"
             "----------------------------------------");
        gpio_conf_t conf = {
            .state = GPIO_OUTPUT_PUSH_PULL,
        };
        expect(0 == gpio_ll_init(port_out, PIN_OUT_0, conf));
        expect(0 == gpio_ll_init(port_out, PIN_OUT_1, conf));

        uword_t both_high = gpio_ll_prepare_write(port_out, mask_both,
                                                  mask_both);
        uword_t both_low = gpio_ll_prepare_write(port_out, mask_both, 0);
        uint32_t start = ztimer_now(ZTIMER_USEC);
        for (uint_fast16_t i = loops; i > 0; i--) {
            gpio_ll_write(port_out, both_high);
            gpio_ll_write(port_out, both_low);
        }
        uint32_t duration = ztimer_now(ZTIMER_USEC) - start;

        if (COMPENSATE_OVERHEAD) {
            print_summary_compensated(loops, duration - loop_overhead,
                                      duration);
        }
        else {
            print_summary_uncompensated(loops, duration);
        }
    }

    puts("\n\nTEST SUCCEEDED");
    return 0;
}
