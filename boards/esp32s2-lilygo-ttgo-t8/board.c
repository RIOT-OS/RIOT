/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32s2_lilygo_ttgo_t8
 * @{
 *
 * @file
 * @brief       Board initialization for the LILYGO TTGO T8 ESP32-S2 board
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    if (IS_USED(MODULE_ST77XX)) {
        gpio_init(BACKLIGHT_PIN, GPIO_OUT);
    }
}
