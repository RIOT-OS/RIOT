/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_ek-lm4f120xl
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Stellaris Launchpad LM4F120 board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include "board.h"
static void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs, this is done for debugging purpose */
    leds_init();

    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief initialize the boards on-boards LEDS.
 */
static void leds_init(void)
{
    // enable clock for PORTF
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //configure the pins as general output
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // Turn Red led on
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 2);
}
