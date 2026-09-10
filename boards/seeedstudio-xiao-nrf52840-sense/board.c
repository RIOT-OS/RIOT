/*
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_seeedstudio-xiao-nrf52840-sense
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Seeedstudio Xiao
 *              nRF52840 Sense board
 *
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"
#include "periph/gpio_ll.h"

void board_init(void)
{
    /* The IMU is supplied through a GPIO Pin (P1.08), so it has to be set
     * to high power mode. */
    gpio_conf_t lsm6ds3_pwr_pin_conf = {0};

    lsm6ds3_pwr_pin_conf.state = GPIO_OUTPUT_PUSH_PULL;      /* Set the output to push pull */
    lsm6ds3_pwr_pin_conf.drive_strength = GPIO_DRIVE_STRONG; /* Enable high drive strength H0H1 */

    /* Power on the IMU if used */
    if (IS_USED(MODULE_LSM6DSXX)) {
        lsm6ds3_pwr_pin_conf.initial_value = true;
    } else {
        lsm6ds3_pwr_pin_conf.initial_value = false;
    }

    gpio_ll_init(gpio_get_port(LSM6DS3_PWR_PIN),
                 gpio_get_pin_num(LSM6DS3_PWR_PIN), lsm6ds3_pwr_pin_conf);
}
