/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_atmega
 * @{
 *
 * @file
 * @brief       Common implementations for Atmega boards
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio.h"

void led_init(void);

void board_init(void)
{
    atmega_stdio_init();
    cpu_init();
    led_init();
    irq_enable();
}
