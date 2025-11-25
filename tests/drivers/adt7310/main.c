/*
 * Copyright (C) 2015 Eistec AB
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
 * @brief       Test application for the ADT7310 accelerometer driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 *
 * @}
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "board.h"
#include "xtimer.h"
#include "periph/spi.h"
#include "adt7310.h"

/* Check for definition of hardware settings */
#ifndef TEST_ADT7310_SPI
#error "TEST_ADT7310_SPI not defined"
#endif
#ifndef TEST_ADT7310_CS
#error "TEST_ADT7310_CS not defined"
#endif

#define SPI_CONF    (SPI_CONF_SECOND_FALLING)
#define SPI_CLK     (SPI_CLK_10MHZ)

#define SLEEP_CONT  (100 * 1000U)
#define SLEEP_1SPS  (1000 * 1000U)
#define READINGS_CONT (200)
#define READINGS_1SPS (20)

int test_adt7310_sample_print(adt7310_t *dev)
{
    int16_t raw;
    float celsius_float;
    int32_t millicelsius;

    raw = adt7310_read_raw(dev);

    if (raw == INT16_MIN) {
        puts("Reading temperature data (adt7310_read_raw)... ");
        puts("[Failed]\n");
        return 1;
    }

    millicelsius = adt7310_read(dev);

    if (millicelsius == INT32_MIN) {
        puts("Reading temperature data (adt7310_read)... ");
        puts("[Failed]\n");
        return 1;
    };

    celsius_float = adt7310_read_float(dev);

    if (isnan(celsius_float)) {
        puts("Reading temperature data (adt7310_read_float)... ");
        puts("[Failed]\n");
        return 1;
    };

    /* Several platforms usually build with nano.specs, (without float printf) */
    /* Split value into two integer parts for printing. */
    float integral = 0;
    float fractional;
    fractional = modff(celsius_float, &integral);

    printf("0x%04" PRIx16 " %7" PRId32 " mC %4d.%07lu C)\n", raw, millicelsius,
        (int)integral, labs((long)(fractional * 10000000.f)));
    return 0;
}

int main(void)
{
    adt7310_t dev;

    puts("ADT7310 temperature driver test application\n");

    puts("Initializing ADT7310 sensor... ");
    if (adt7310_init(&dev, TEST_ADT7310_SPI, SPI_CLK, TEST_ADT7310_CS) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("ADT7310 init done.\n");

    while (1) {
        puts("Set mode to continuous, 16 bit... ");
        if (adt7310_set_config(&dev, ADT7310_MODE_CONTINUOUS | ADT7310_CONF_RESOLUTION(1)) == 0) {
            puts("[OK]");
        }
        else {
            puts("[Failed]\n");
            return 1;
        }

        for (int i = 0; i < READINGS_CONT; ++i)
        {
            printf("%4d: ", i);
            if (test_adt7310_sample_print(&dev) != 0)
            {
                return 1;
            }
            xtimer_usleep(SLEEP_CONT);
        }
        puts("Set mode to 1SPS, 13 bit... ");
        if (adt7310_set_config(&dev, ADT7310_MODE_1SPS) == 0) {
            puts("[OK]");
        }
        else {
            puts("[Failed]\n");
            return 1;
        }

        for (int i = 0; i < READINGS_1SPS; ++i)
        {
            printf("%4d: ", i);
            if (test_adt7310_sample_print(&dev) != 0)
            {
                return 1;
            }
            xtimer_usleep(SLEEP_1SPS);
        }
    }

    return 0;
}
