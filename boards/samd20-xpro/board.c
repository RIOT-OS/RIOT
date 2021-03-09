/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samd20-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM D20 Xplained
 *              Pro board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;

    /* initialize the on-board button */
    gpio_init(BTN0_PIN, BTN0_MODE);

}
