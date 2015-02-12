/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_openmote
 * @{
 *
 * @file
 * @brief       Board specific implementations for the OpenMote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

static inline void leds_init(void);

void board_init(void)
{
    /* initialize the boards LEDs */
    leds_init();
    /* initialize the CPU */
    cpu_init();
}

/**
 * @brief Initialize the boards on-board LEDs (LD3 and LD4)
 *
 * The LED initialization is hard-coded in this function. As the LEDs are soldered
 * onto the board they are fixed to their CPU pins.
 *
 * The LEDs are connected to the following pins:
 * - LED1: PC4 (red)
 * - LED2: PC5 (orange)
 * - LED3: PC6 (yellow)
 * - LED4: PC7 (green)
 */
static inline void leds_init(void)
{
    /* set pins to be controlled by software */
    LED_PORT->AFSEL &= ~((1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) |
                         (1 << LED_YELLOW_PIN) | (1 << LED_ORANGE_PIN));
    /* configure pins as output */
    LED_PORT->DIR |= ((1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) |
                      (1 << LED_YELLOW_PIN) | (1 << LED_ORANGE_PIN));
    /* configure io-mux for used pins */
    IOC->PC_OVER[LED_RED_PIN] = IOC_OVERRIDE_OE;
    IOC->PC_OVER[LED_GREEN_PIN] = IOC_OVERRIDE_OE;
    IOC->PC_OVER[LED_YELLOW_PIN] = IOC_OVERRIDE_OE;
    IOC->PC_OVER[LED_ORANGE_PIN] = IOC_OVERRIDE_OE;
}
