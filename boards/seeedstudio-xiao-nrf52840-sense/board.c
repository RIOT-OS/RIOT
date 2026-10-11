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
    /* The IMU is supplied through a GPIO Pin (P1.08) and the microphone through
     * Pin (P1.10), so they have to be set to high power mode. */
    gpio_conf_t periph_pwr_pin_conf = { 0 };

    periph_pwr_pin_conf.state = GPIO_OUTPUT_PUSH_PULL;      /* Set the output to push pull */
    periph_pwr_pin_conf.drive_strength = GPIO_DRIVE_STRONG; /* Enable high drive strength H0H1 */

    /* Power on the IMU if used */
    if (IS_USED(MODULE_LSM6DSXX)) {
        periph_pwr_pin_conf.initial_value = true;
    }
    else {
        periph_pwr_pin_conf.initial_value = false;
    }

    gpio_ll_init(gpio_get_port(LSM6DS3_PWR_PIN),
                 gpio_get_pin_num(LSM6DS3_PWR_PIN), periph_pwr_pin_conf);

    /* Power on the PDM microphone if used */
    if (IS_USED(MODULE_PERIPH_PDM)) {
        periph_pwr_pin_conf.initial_value = true;
    }
    else {
        periph_pwr_pin_conf.initial_value = false;
    }

    gpio_ll_init(gpio_get_port(PDM_PWR_PIN),
                 gpio_get_pin_num(PDM_PWR_PIN), periph_pwr_pin_conf);
}
