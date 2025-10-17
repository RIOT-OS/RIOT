/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for a generic incremental rotary encoder
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "inc_encoder.h"
#include "inc_encoder_params.h"
#include "ztimer.h"

static inc_encoder_t dev;

int main(void)
{
    puts("Generic incremental rotary encoder test application\n");
    if (inc_encoder_init(&dev, &inc_encoder_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    puts("Printing sensor state every second.");
    while (1) {
        int32_t rpm;
        int32_t revs;
        if (inc_encoder_read_rpm(&dev, &rpm)
            || inc_encoder_read_reset_milli_revs(&dev, &revs)) {
            puts("[Failed]");
            return 1;
        }
        printf("SENSOR DATA:\n\tRPM  : %ld\n\tMREVS: %ld\n", (long) rpm, (long) revs);
        ztimer_sleep(ZTIMER_SEC, 1);
    }
}
