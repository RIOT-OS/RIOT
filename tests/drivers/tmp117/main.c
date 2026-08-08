/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the TMP117 sensor driver
 *
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#include "tmp117.h"
#include "tmp117_params.h"


int main(void)
{
    tmp117_t tmp117;
    int16_t raw_temp;

    puts("Test for the TMP117 temperature sensor\n");

    puts("Initializing tmp117 sensor...");

    if (tmp117_init(&tmp117, tmp117_params) != TMP117_OK) {
        puts("  failed.\n");
        return -1;
    }

    puts("  done.\n");

    while (1) {
        if (tmp117_read_temperature(&tmp117, &raw_temp) == TMP117_OK) {
            printf("Temperature : %02d.%02d°C\n", raw_temp / 100, raw_temp % 100);
        }
        else {
            puts("ERROR: unable to read temperature\n");
            break;
        }

        ztimer_sleep(ZTIMER_MSEC, 1500);
    }


    return 0;
}
