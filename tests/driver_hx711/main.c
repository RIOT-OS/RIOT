/*
 * Copyright (C) 2019 Philipp-Alexander Blum
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
 * @brief       Test application for HX711 scale weight sensor
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "hx711.h"
#include "hx711_params.h"
#include "xtimer.h"
#include "board.h"

int main(void)
{
    puts("HX711 test application\n");

    printf("+------------Initializing------------+\n");
    hx711_init();

    puts("Initialization successful");

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        printf("Unit values: %lu"
               "\n+-------------------------------------+\n",
               hx711_get_units(40));

        /* 2s delay before next measure*/
        xtimer_sleep(2);
    }

    return 0;
}
