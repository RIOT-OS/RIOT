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

#ifndef CPU_ATMEGA_CLK_SCALE_INIT
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
#endif

void led_init(void);

void __attribute__((weak)) board_init(void)
{
#ifdef CPU_ATMEGA32U4
    /* disable usb interrupt on Atmega32U4 */
    PRR1 |= 1<<PRUSB;
#endif

    atmega_set_prescaler(CPU_ATMEGA_CLK_SCALE_INIT);

    cpu_init();
#ifdef LED0_ON
    led_init();
#endif
}
