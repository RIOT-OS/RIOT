/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ublox-c030-u201
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Ublox C030-U201 board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
}
