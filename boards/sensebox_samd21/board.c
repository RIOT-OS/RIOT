/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_sensebox_samd21
 * @{
 *
 * @file
 * @brief       Board specific implementations for the SenseBox MCU with SAMD21
 * board.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize XBEE1 bus enable pin */
    gpio_init(XBEE1_EN_PIN, XBEE1_EN_MODE);

    /* enable XBEE1 bus */
    XBEE1_ENABLE;

    /* initialize XBEE2 bus enable pin */
    gpio_init(XBEE2_EN_PIN, XBEE2_EN_MODE);

    /* enable XBEE2 bus */
    XBEE2_ENABLE;

    /* initialize I2C bus enable pin */
    gpio_init(I2C_EN_PIN, I2C_EN_MODE);

    /* enable I2C bus */
    I2C_ENABLE;

    /* delay for SPI reset. Read README.md for more information. */
    for (unsigned i = 0; i < WAIT_FOR_SPI_RESET; i++) {
        __asm__("nop");
    }
}
