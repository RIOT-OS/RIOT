/*
 * Copyright (C) 2015-2016 Eistec AB
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
 * @brief       Test application for the ADXL362 sensor driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifndef TEST_ADXL362_SPI
#error "TEST_ADXL362_SPI not defined"
#endif
#ifndef TEST_ADXL362_CS
#error "TEST_ADXL362_CS not defined"
#endif

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "timex.h"
#include "adxl362.h"

#define SLEEP    (100 * MS_IN_USEC)

int main(void)
{
    int res;
    adxl362_t dev;

    puts("ADXL362 sensor driver test application\n");
    printf("Initializing SPI_%i... ", TEST_ADXL362_SPI);
    /* ADXL362 requires SPI Mode 0 */
    /* Max fSCK is 1 MHz, according to the data sheet, but the device seem to
     * work fine with higher speeds as well. */
    res = spi_init_master(TEST_ADXL362_SPI, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
    if (res < 0) {
        puts("[Failed]");
        return -1;
    }
    puts("[OK]");

    printf("Initializing ADXL362 sensor at SPI_%i, CS 0x%04x... ",
        TEST_ADXL362_SPI, TEST_ADXL362_CS);
    res = adxl362_init(&dev, TEST_ADXL362_SPI, TEST_ADXL362_CS);
    if (res < 0) {
        puts("[failed]");
        return -1;
    }
    puts("[OK]");

    uint32_t last_wakeup = xtimer_now();
    printf("Performing sensor self-test...");
    res = adxl362_self_test(&dev);
    if (res < 0) {
        puts("[Error!]");
    }
    else if (res > 0) {
        puts("[OK]");
    }
    else {
        puts("[failed]");
    }

    printf("Enable ADXL362 FIFO... ");
    res = adxl362_set_fifo(&dev, false, ADXL362_FIFO_MODE_LAST_N, ADXL362_FIFO_WATERMARK_DEFAULT);
    if (res < 0) {
        puts("[failed]");
        return -1;
    }
    puts("[OK]");

    while (1) {
        adxl362_data_t values[32];
        int status;

        xtimer_periodic_wakeup(&last_wakeup, SLEEP);

        int32_t temp = 0;
        status = adxl362_read_temp(&dev, &temp);
        if (status < 0) {
            printf("Communication error: %d\n", status);
            continue;
        }
        printf("temp = %6" PRId32 "\n", temp);

        status = adxl362_read_acc_fifo(&dev, values, sizeof(values) / sizeof(values[0]));
        if (status < 0) {
            printf("Communication error: %d\n", status);
            continue;
        } else if (status == 0) {
            puts("Empty FIFO");
            continue;
        }
        for (int i = 0; i < status; ++i)
        {
            printf("%4d: x: %6d y: %6d z: %6d\n", i,
                (int)values[i].acc_x, (int)values[i].acc_y, (int)values[i].acc_z);
        }
    }

    return 0;
}
