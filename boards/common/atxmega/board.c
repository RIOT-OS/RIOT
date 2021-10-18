/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
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
 * @brief       Common implementations for ATxmega boards
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#ifdef LED_PORT
void __attribute__((weak)) led_init(void)
{
    LED_PORT.DIR = LED_PORT_MASK;
    LED_PORT.OUT = LED_PORT_MASK;
}
#endif

void __attribute__((weak)) board_init(void)
{
#ifdef LED_PORT
    led_init();
#endif
}
