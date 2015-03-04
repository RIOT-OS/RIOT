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
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "vtimer.h"
#include "periph/spi.h"
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


#define SCALE       LIS3DH_SCALE_4G
#define ODR         LIS3DH_ODR_100Hz
#define SLEEP       (100 * 1000U)
#define SPI_CONF    (SPI_CONF_SECOND_FALLING)

int main(void)
{
    lis3dh_t dev;
    lis3dh_data_t acc_data;
    int16_t temperature;

    puts("LIS3DH accelerometer driver test application\n");
    printf("Initializing SPI_%i... ", TEST_LIS3DH_SPI);
    if (spi_init_master(TEST_LIS3DH_SPI, SPI_CONF, SPI_SPEED_10MHZ) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]\n");
        return 1;
    }

    puts("Initializing LIS3DH sensor... ");
    if (lis3dh_init(&dev, TEST_LIS3DH_SPI, TEST_LIS3DH_CS,
                      TEST_LIS3DH_INT1, TEST_LIS3DH_INT2, SCALE) == 0) {
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

    puts("Disable FIFO mode... ");
    if (lis3dh_set_fifo(&dev, 0) == 0) {
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

    puts("LIS3DH init done.\n");

    while (1) {
        lis3dh_read_xyz(&dev, &acc_data);
        if (lis3dh_read_xyz(&dev, &acc_data) != 0) {
            puts("Reading acceleration data... ");
            puts("[Failed]\n");
            return 1;
        }
        if (lis3dh_read_aux_adc3(&dev, &temperature) != 0) {
            puts("Reading temperature data... ");
            puts("[Failed]\n");
            return 1;
        }

        printf("Sensor data - X: %6i   Y: %6i   Z: %6i   Temp: %6i\n",
               acc_data.acc_x, acc_data.acc_y, acc_data.acc_z, temperature);

        vtimer_usleep(SLEEP);
    }

    return 0;
}
