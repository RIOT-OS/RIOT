/*
 * Copyright (C) 2015 nqd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_cc3200-lp
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the CC3200 Launchpad
 *
 * @author      nqd
 *
 * @}
 */

#include "board.h"
static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs, this is done for debugging purpose */
    leds_init(); 
}

/**
 * @brief initialize the boards on-boards LEDS.
 */
static void leds_init(void)
{
    /* enable clock for PORTF */
    // ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /* configure the pins as general output */
    // ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}
