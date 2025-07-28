/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_samd10-xmini
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM D10 Xplained
 *              Mini board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* If the PA24 and PA25 pins are not connected, it is recommended
     * to enable a pull-up on PA24 and PA25 through input GPIO mode.
     * (those have external pull-ups on the board that would leak current)
     */
    gpio_init(GPIO_PIN(PA, 24), GPIO_IN_PU);
    gpio_init(GPIO_PIN(PA, 25), GPIO_IN_PU);
}
