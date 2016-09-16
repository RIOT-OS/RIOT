/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2016 OTA keys S.A.
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
 * @brief       Test application for the MMA8652 accelerometer driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 *
 * @}
 */

#ifndef TEST_MMA8652_I2C
#error "TEST_MMA8652_I2C not defined"
#endif
#ifndef TEST_MMA8652_ADDR
#error "TEST_MMA8652_ADDR not defined"
#endif
#ifndef TEST_MMA8x5x_TYPE
#error "TEST_MMA8x5x_TYPE not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "mma8652.h"
#include "mma8652_reg.h"

#ifdef MODULE_MMA8652_INT
#define MMA8652_INT1_PIN            GPIO_PIN(PORT_B, 0)
#define MMA8652_INT2_PIN            GPIO_PIN(PORT_B, 1)

#define INT_PIN1                    (0)
#define INT_PIN2                    (1)
#define NB_INT_PIN                  (2)

extern void cb_int1(void *arg);
extern void cb_int2(void *arg);

static mma8652_int_t mma8652_interrupt[NB_INT_PIN] =
{
    {
        .pin = MMA8652_INT1_PIN,
        .source = MMA8652_INT_SOURCE_DRDY, // MMA8652_INT_SOURCE_NONE
        .cb = cb_int1,
        .arg = NULL,
    },
    {
        .pin = MMA8652_INT2_PIN,
        .source = MMA8652_INT_SOURCE_NONE, // MMA8652_INT_SOURCE_DRDY
        .cb = cb_int2,
        .arg = NULL,
    },
};

volatile static bool mma8652_semaphore = false;

void cb_int1(void *arg)
{
    (void) arg;
    printf("MMA8652 INT 1\n");
    mma8652_semaphore = true;
}

void cb_int2(void *arg)
{
    (void) arg;
    printf("MMA8652 INT 2\n");
    mma8652_semaphore = true;
}
#endif

#define SLEEP       (1000 * 1000U)

int main(void)
{
    mma8652_t dev;
    int16_t x, y, z;
    uint8_t status;

    puts("MMA8652 accelerometer driver test application\n");
    printf("Initializing MMA8652 accelerometer at I2C_%i... ", TEST_MMA8652_I2C);

#ifdef MODULE_MMA8652_INT
    printf("with interruption... ");
    if (mma8652_init_int(&dev, TEST_MMA8652_I2C, TEST_MMA8652_ADDR,
                     MMA8652_DATARATE_DEFAULT,
                     MMA8652_FS_RANGE_DEFAULT,
                     TEST_MMA8x5x_TYPE,
                     &(mma8652_interrupt[INT_PIN1]),
                     &(mma8652_interrupt[INT_PIN2])) == 0) {
#else
    if (mma8652_init(&dev, TEST_MMA8652_I2C, TEST_MMA8652_ADDR,
                     MMA8652_DATARATE_DEFAULT,
                     MMA8652_FS_RANGE_DEFAULT,
                     TEST_MMA8x5x_TYPE) == 0) {
#endif
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (mma8652_set_user_offset(&dev, TEST_MMA8652_USER_OFFSET_X,
                                TEST_MMA8652_USER_OFFSET_Y,
                                TEST_MMA8652_USER_OFFSET_Z)) {
        puts("Set user offset correction failed.");
        return -1;
    }

    if (mma8652_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
#ifdef MODULE_MMA8652_INT
        if (mma8652_semaphore == true) {
            uint8_t int_status = 0;
            int res = -1;

            res = mma8682_read_interruption_status(&dev, &int_status);
            if (res != 0) {
                puts("Read interruption status failed.");
                return -1;
            }

            if ((int_status & MMA8652_INT_SOURCE_DRDY) == MMA8652_INT_SOURCE_DRDY) {
                mma8652_read(&dev, &x, &y, &z, &status);
                printf("Acceleration, in mg: X: %d Y: %d Z: %d S: %2x\n", x, y, z, status);
                mma8652_semaphore = false;
            }
        }
#else
        mma8652_read(&dev, &x, &y, &z, &status);
        printf("Acceleration, in mg: X: %d Y: %d Z: %d S: %2x\n", x, y, z, status);
#endif
        xtimer_usleep(SLEEP);
    }

    return 0;
}
