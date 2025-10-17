/*
* SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for a generic hall effect sensor
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>

#include "hall_effect.h"
#include "hall_effect_params.h"
#include "ztimer.h"

static hall_effect_t dev;

int main(void)
{
    puts("Generic hall effect sensor test application\n");
    printf("Initializing hall effect sensor with the interrupt pin at "
           "GPIO_%d and the direction pin at GPIO_%d...\n",
           HALL_EFFECT_INTERRUPT, HALL_EFFECT_DIRECTION);
    if (hall_effect_init(&dev, &hall_effect_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    puts("Printing sensor state every second.");
    while (1) {
        int32_t rpm;
        int32_t pulses;
        if (hall_effect_read_rpm(&dev, &rpm)
            || hall_effect_read_reset_ceti_revs(&dev, &pulses)) {
            puts("[Failed]");
            return 1;
        }
        printf("SENSOR DATA:\n\tRPM: %ld\n\tPULSES: %ld\n", rpm, pulses);
        ztimer_sleep(ZTIMER_SEC, 1);
    }
}
