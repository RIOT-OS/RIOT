/*
 * Copyright (C) 2019 Marius <marius@twostairs.com>
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
 * @brief       Test application for the PCA9685 16-channel, 12-bit PWM Fm+ I²C-bus controller driver.
 *
 * @author      Marius <marius@twostairs.com>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#include "pca9685.h"

int main(void)
{
    pca9685_t pca9685;
    pca9685_params_t pca9685_params;

    pca9685_params.pwm_freq = 50.0;
    pca9685_params.i2c_dev = PCA9685_PARAM_I2C_DEV;
    pca9685_params.i2c_addr = PCA9685_PARAM_I2C_ADDR;

    printf("Initializing ...\n");
    if(pca9685_init(&pca9685, &pca9685_params) != PCA9685_OK) {
        printf("Error initializing PCA9685!\n");
        return -1;
    }

    printf("Waking up ...\n");
    if(pca9685_wakeup(&pca9685) != PCA9685_OK) {
        printf("Error waking up PCA9685!\n");
        return -1;
    }

    printf("Setting PWM ...\n");
    if(pca9685_set_pwm(&pca9685, 0, 200, 200, true, false) != PCA9685_OK) {
        printf("Error setting PWM on PCA9685!\n");
        return -1;
    }

    if(pca9685_get_pwm(&pca9685, 0) != 200) {
        printf("Something is odd here.\n");
        return -1;
    }

    return 0;
}
