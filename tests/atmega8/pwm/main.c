/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2023 Hugues Larrive
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
 * @brief       Test for low-level PWM drivers
 *
 * This test initializes the given PWM device to run at 1KHz with a 1000 step
 * resolution.
 *
 * The PWM is then continuously oscillating it's duty cycle between 0% to 100%
 * every 1s on every channel.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "periph/pwm.h"

#define F_CPU                   (CLOCK_CORECLOCK)
#define __DELAY_BACKWARD_COMPATIBLE__
#include "util/delay.h"

#define OSC_INTERVAL    (400) /* 400us */
#define OSC_STEP        (10)
#define OSC_MODE        PWM_LEFT
#define OSC_FREQU       (1000U)
#define OSC_STEPS       (256U)
#define PWR_SLEEP       (1U)

static uint32_t initialized;

static int _oscillate(void)
{
    int state = 0;
    int step = OSC_STEP;

    puts("\nRIOT PWM test");
    puts("Connect an LED or scope to PWM pins to see something.\n");

    printf("Available PWM device between 0 and %d\n", PWM_NUMOF - 1);
    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        uint32_t real_f = pwm_init(PWM_DEV(i), OSC_MODE, OSC_FREQU, OSC_STEPS);
        if (real_f == 0) {
            printf("Error: initializing PWM_%u.\n", i);
            return 1;
        }
        else {
            printf("Initialized PWM_%u @ %" PRIu32 "Hz.\n", i, real_f);
        }
    }

    puts("\nLetting the PWM pin (PB3 on atmega8) oscillate now...");
    while (1) {
        for (unsigned i = 0; i < PWM_NUMOF; i++) {
            for (uint8_t chan = 0; chan < pwm_channels(PWM_DEV(i)); chan++) {
                pwm_set(PWM_DEV(i), chan, state);
            }
        }

        state += step;
        if (state <= 0 || state >= (int)OSC_STEPS) {
            step = -step;
        }

        _delay_us(OSC_INTERVAL);
    }

    return 0;
}

int main(void)
{
    puts("PWM peripheral driver test\n");
    initialized = 0;

    _oscillate();

    return 0;
}
