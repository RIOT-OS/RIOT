/*
 * Copyright (C) 2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
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
 * @brief       HX711 scale ADC driver test application
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "hx711.h"
#include "hx711_params.h"

#include "test_utils/expect.h"

static hx711_t dev;
uint8_t times = 200;

int main(void)
{
    puts("HX711 test application\n");
    puts("+------------Initializing------------+");
    hx711_init(&dev, &(hx711_params[0]));
    hx711_power_up(&dev);
    puts("Initialization successful\n\n");

    puts("+--------Starting Measurements--------+");
    int32_t value_before = hx711_get_units(&dev, times);
    printf("value before taring: %"PRIu32"\n", value_before);
    hx711_tare(&dev, times);
    int32_t value_after = hx711_get_units(&dev, times);
    printf("value after taring: %"PRIu32"\n", value_after);

    expect(value_before > value_after);
    expect(value_after <= 1);

    return 0;
}
