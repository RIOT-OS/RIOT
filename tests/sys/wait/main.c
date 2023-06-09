/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for the sys/wait
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "clk.h"
#include "irq.h"
#include "led.h"
#include "time_units.h"
#include "wait.h"
#include "ztimer.h"

static void estimate_cpu_cycle_per_iteration_of_wait_loop(void)
{
    ztimer_acquire(ZTIMER_USEC);
    const uint32_t iterations = 100000;
    uint32_t duration_us = ztimer_now(ZTIMER_USEC);
    wait_busy_loop(iterations);
    duration_us = ztimer_now(ZTIMER_USEC) - duration_us;
    ztimer_release(ZTIMER_USEC);
    uint32_t duration_cycles = duration_us * (uint64_t)coreclk() / US_PER_SEC;
    uint32_t cycles_per_iteration = duration_cycles / iterations;
    printf("CPU_CYCLES_PER_WAIT_LOOP_ITERATION actual:   %" PRIu32 "\n",
           (uint32_t)CPU_CYCLES_PER_WAIT_LOOP_ITERATION);
    printf("CPU_CYCLES_PER_WAIT_LOOP_ITERATION detected: %" PRIu32 "\n",
           cycles_per_iteration);
}

int main(void)
{
    printf(
        "Test Application for sys/wait\n"
        "==============================\n"
        "\n"
    );

    estimate_cpu_cycle_per_iteration_of_wait_loop();

    printf("Waiting: 1 s\n");
    wait_at_least_s(1);
    printf("Done\n");

    printf("Waiting: 100 ms\n");
    wait_at_least_ms(100);
    printf("Done\n");

    printf("Waiting: 10000 us\n");
    wait_at_least_us(10000);
    printf("Done\n");

    printf("Now blinking LED with a 100 μs pulse every 100 ms\n");
    while (1) {
        unsigned state = irq_disable();
        LED0_ON;
        wait_us_spinning(100);
        LED0_OFF;
        irq_restore(state);
        wait_at_least_ms(100);
    }
    return 0;
}
