/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the LIS3DH accelerometer driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "lis3dh.h"
#include "lis3dh_params.h"

#define SLEEP_USEC  (100 * 1000U)

#define WATERMARK_LEVEL 16

static volatile int int1_count = 0;

static void test_int1(void *arg)
{
    volatile int *int1_count_ptr = arg;
    ++(*int1_count_ptr);
}

int main(void)
{
    lis3dh_t dev;
    lis3dh_data_t acc_data;

    puts("LIS3DH accelerometer driver test application\n");

    puts("Initializing LIS3DH sensor... ");
    if (lis3dh_init(&dev, &lis3dh_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set ODR... ");
    if (lis3dh_set_odr(&dev, lis3dh_params[0].odr) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set scale... ");
    if (lis3dh_set_scale(&dev, lis3dh_params[0].scale) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set axes XYZ... ");
    if (lis3dh_set_axes(&dev, LIS3DH_AXES_XYZ) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Enable streaming FIFO mode... ");
    if (lis3dh_set_fifo(&dev, LIS3DH_FIFO_MODE_STREAM, WATERMARK_LEVEL) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Enable temperature reading... ");
    if (lis3dh_set_aux_adc(&dev, 1, 1) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set INT1 watermark function... ");
    if (lis3dh_set_int1(&dev, LIS3DH_CTRL_REG3_I1_WTM_MASK) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set INT1 callback");
    if (gpio_init_int(lis3dh_params[0].int1, GPIO_IN, GPIO_RISING,
                      test_int1, (void*)&int1_count) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("LIS3DH init done.\n");

    while (1) {
        int fifo_level;

        fifo_level = lis3dh_get_fifo_level(&dev);
        printf("int1_count = %d\n", int1_count);
        printf("Reading %d measurements\n", fifo_level);
        while (fifo_level > 0) {
            int16_t temperature;
            int int1;
            if (lis3dh_read_xyz(&dev, &acc_data) != 0) {
                puts("Reading acceleration data... ");
                puts("[Failed]\n");
            }
            if (lis3dh_read_aux_adc3(&dev, &temperature) != 0) {
                puts("Reading temperature data... ");
                puts("[Failed]\n");
                return 1;
            }
            int1 = gpio_read(lis3dh_params[0].int1);
            printf("X: %6d Y: %6d Z: %6d Temp: %6d, INT1: %08x\n",
                   acc_data.acc_x, acc_data.acc_y, acc_data.acc_z, temperature, int1);
            --fifo_level;
        }

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
