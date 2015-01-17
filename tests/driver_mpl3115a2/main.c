/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 * @brief       Test application for the MPL3115A2 sensor driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#ifndef TEST_MPL3115A2_I2C
#error "TEST_MPL3115A2_I2C not defined"
#endif
#ifndef TEST_MPL3115A2_ADDR
#error "TEST_MPL3115A2_ADDR not defined"
#endif

#include <stdio.h>

#include "vtimer.h"
#include "mpl3115a2.h"

#define SLEEP       (1000 * 1000U)

int main(void)
{
    mpl3115a2_t dev;
    uint32_t pressure;
    int16_t temp;
    uint8_t status;

    puts("MPL3115A2 pressure sensor driver test application\n");
    printf("Initializing MPL3115A2 sensor at I2C_%i... ", TEST_MPL3115A2_I2C);
    if (mpl3115a2_init(&dev, TEST_MPL3115A2_I2C, TEST_MPL3115A2_ADDR,
                                                 MPL3115A2_OS_RATIO_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (mpl3115a2_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
        vtimer_usleep(SLEEP);
        mpl3115a2_read_pressure(&dev, &pressure, &status);
        printf("Pressure: %u Status: %#02x\n", (unsigned int)pressure, status);
        mpl3115a2_read_temp(&dev, &temp);
        printf("Temperature: %d\n", temp/10);
    }

    return 0;
}
