/*
 * SPDX-FileCopyrightText: 2026 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_ulanzi-tc001
 * @{
 *
 * @file
 * @brief       Board specific implementations for Ulanzi TC001 digital Clock
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* silence buzzer */
    gpio_init(GPIO15, GPIO_OUT);
}
