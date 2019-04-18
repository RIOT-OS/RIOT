/*
 * Copyright (C) 2019 Philipp-Alexander Blum
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       HX711 Calibration example
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hx711.h"
#include "xtimer.h"

int main(void)
{
    puts("RIOT HX711 Scale calibration");
    puts("=====================================");

    puts("Init HX711...");
    hx711_init();
    puts("Initialized HX711.");

    while(true){
        printf("Read units: %li\n", hx711_get_units(40));
        xtimer_sleep(2);
    }

    return 0;
}
