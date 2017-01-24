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
 * @brief       Test application for the LIS3DH accelerometer driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "xtimer.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "lis3dh.h"

/* Check for definition of hardware pins, default to board.h values if not set. */
#ifndef TEST_LIS3DH_SPI
#ifdef LIS3DH_SPI
#define TEST_LIS3DH_SPI LIS3DH_SPI
#else
#error "TEST_LIS3DH_SPI not defined"
#endif
#endif
#ifndef TEST_LIS3DH_CS
#ifdef LIS3DH_CS
#define TEST_LIS3DH_CS LIS3DH_CS
#else
#error "TEST_LIS3DH_CS not defined"
#endif
#endif
#ifndef TEST_LIS3DH_INT1
#ifdef LIS3DH_INT1
#define TEST_LIS3DH_INT1 LIS3DH_INT1
#else
#error "TEST_LIS3DH_INT1 not defined"
#endif
#endif
#ifndef TEST_LIS3DH_INT2
#ifdef LIS3DH_INT2
#define TEST_LIS3DH_INT2 LIS3DH_INT2
#else
#error "TEST_LIS3DH_INT2 not defined"
#endif
#endif


#define SCALE       4
#define ODR         LIS3DH_ODR_100Hz
#define SLEEP       (100 * 1000U)
#define SPI_CONF    (SPI_CONF_SECOND_FALLING)
#define SPI_SPEED   (SPI_SPEED_10MHZ)

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
    printf("Initializing SPI_%i... ", TEST_LIS3DH_SPI);
    if (spi_init_master(TEST_LIS3DH_SPI, SPI_CONF, SPI_SPEED) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Initializing LIS3DH sensor... ");
    if (lis3dh_init(&dev, TEST_LIS3DH_SPI, TEST_LIS3DH_CS, SCALE) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set ODR... ");
    if (lis3dh_set_odr(&dev, ODR) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Set scale... ");
    if (lis3dh_set_scale(&dev, SCALE) == 0) {
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
    if (gpio_init_int(TEST_LIS3DH_INT1, GPIO_IN, GPIO_RISING, test_int1, (void*)&int1_count) == 0) {
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
            int1 = gpio_read(TEST_LIS3DH_INT1);
            printf("X: %6d Y: %6d Z: %6d Temp: %6d, INT1: %08x\n",
                   acc_data.acc_x, acc_data.acc_y, acc_data.acc_z, temperature, int1);
            --fifo_level;
        }

        xtimer_usleep(SLEEP);
    }

    return 0;
}
