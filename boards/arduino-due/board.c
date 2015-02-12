/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-due
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Arduino Due board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"


void led_init(void);


void board_init(void)
{
    /* initialize core clocks via CMSIS function provided by Atmel */
    SystemInit();

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
}


/**
 * @brief Initialize the boards on-board LED (Amber LED "L")
 *
 * The LED initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LED is connected to the following pin:
 * - LED: PB27
 */
void led_init(void)
{
    /* enable PIO control of pin PD27 */
    LED_PORT->PIO_PER = LED_PIN;
    /* set pin as output */
    LED_PORT->PIO_OER = LED_PIN;
    /* enable direct write access to the LED pin */
    LED_PORT->PIO_OWER = LED_PIN;
    /* disable pull-up */
    LED_PORT->PIO_PUDR = LED_PIN;
    /* clear pin */
    LED_PORT->PIO_CODR = LED_PIN;
}
