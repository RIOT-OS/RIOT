/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common
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
#include "periph/gpio.h"

void led_init(void);

void __attribute__((weak)) board_init(void)
{
#ifdef LED0_ON
    led_init();
#endif
}
