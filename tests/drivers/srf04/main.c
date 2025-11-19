/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for srf04 ultra sonic range finder driver
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srf04_params.h"
#include "srf04.h"

int main(void)
{
    puts("SRF04 range finder example");

    srf04_t dev;
    if (srf04_init(&dev, &srf04_params[0]) != SRF04_OK) {
        puts("Error: initializing");
        return 1;
    }

    while (1) {
        int distance = srf04_get_distance(&dev);
        if (distance < SRF04_OK) {
            puts("Error: no valid data available");
        }
        else {
            printf("D: %d mm\n", distance);
        }
    }

    return 0;
}
