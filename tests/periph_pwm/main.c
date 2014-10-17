/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level PWM drivers
 *
 * This test initializes the given PWM device to run at 1KHz with a 1000 step resolution.
 *
 * The PWM is then continuously oscillating it's duty cycle between 0% to 100% every 1s on
 * every channel.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/pwm.h"

#define WAIT        (10000)
#define STEP        (10)

#define DEV         PWM_0
#define CHANNELS    PWM_0_CHANNELS
#define MODE        PWM_LEFT

#define FREQU       (1000U)
#define STEPS       (1000U)


int main(void)
{
    int res;
    int state = 0;
    int step = STEP;

    puts("\nRIOT PWM test");
    puts("Connect an LED or scope to PWM pins to see something\n");

    res = pwm_init(DEV, MODE, FREQU, STEPS);
    if (res == 0) {
        puts("PWM successfully initialized.\n");
    }
    else {
        puts("Errors while initializing PWM");
        return -1;
    }

    while (1) {
        for (int i = 0; i < CHANNELS; i++) {
            pwm_set(DEV, i, state);
        }

        state += step;
        if (state <= 0 || state >= STEPS) {
            step = -step;
        }

        vtimer_usleep(WAIT);
    }

    return 0;
}
