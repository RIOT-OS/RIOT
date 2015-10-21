/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "periph/pwm.h"

#define INTERVAL    (10000U)
#define STEP        (10)

#define MODE        PWM_LEFT
#define FREQU       (1000U)
#define STEPS       (1000U)


int main(void)
{
    int state = 0;
    int step = STEP;
    uint32_t last_wakeup = xtimer_now();

    puts("\nRIOT PWM test");
    puts("Connect an LED or scope to PWM pins to see something\n");

    printf("Available PWM devices: %i\n", PWM_NUMOF);
    for (int i = 0; i < PWM_NUMOF; i++) {
        uint32_t real_f = pwm_init(PWM_DEV(i), MODE, FREQU, STEPS);
        if (real_f == 0) {
            printf("Error initializing PWM_%i\n", i);
            return 1;
        }
        else {
            printf("Initialized PWM_%i @ %" PRIu32 "Hz\n", i, real_f);
        }
    }

    puts("\nLetting the PWM pins oscillate now...");
    while (1) {
        for (int i = 0; i < PWM_NUMOF; i++) {
            for (uint8_t chan = 0; chan < pwm_channels(PWM_DEV(i)); chan++) {
                pwm_set(PWM_DEV(i), chan, state);
            }
        }

        state += step;
        if (state <= 0 || state >= STEPS) {
            step = -step;
        }

        xtimer_usleep_until(&last_wakeup, INTERVAL);
    }

    return 0;
}
