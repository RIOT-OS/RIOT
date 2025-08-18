/*
 * SPDX-FileCopyrightText: 2024 Méwen Berthelot
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_arduino-nano-33-ble-sense
 * @{
 *
 * @file
 * @brief       Board specific initialization for Arduino Nano 33 BLE sense
 *
 * @author      Méwen Berthelot <berthelotmewen@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{

/*
 *  VDD needs to be set in order to have the sensors powered.
 *  It also seems that if the internal I2C pins are not set as input,
 *  all the sensors connected will not work.
 */
    gpio_init(VDD, GPIO_OUT);
    gpio_set(VDD);

    gpio_init(SCL1, GPIO_IN);
    gpio_init(SDA1, GPIO_IN);

}
