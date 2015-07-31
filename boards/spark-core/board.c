/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_spark-core
 * @{
 *
 * @file
 * @brief       Board specific implementations for the spark-core board
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

static void leds_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
    /* initialize the boards LEDs */
    leds_init();
    /* disable systick interrupt, set by the spark bootloader */
    SysTick->CTRL = 0;
    /* signal to spark bootloader: do not enable IWDG! set Stop Mode Flag! */
    BKP->DR9 = 0xAFFF;
    /* configure the RIOT vector table location to internal flash + spark bootloader offset */
    SCB->VTOR = LOCATION_VTABLE;
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LEDs initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 */
static void leds_init(void)
{
    /* enable clock for port A */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* reset pins */
    LED_PORT->CR[1] &= ~(0xf << (4*(LED_RED_PIN - 8)) |
                       0xf << (4*(LED_GREEN_PIN - 8)) |
                       0xf << (4*(LED_BLUE_PIN - 8)) |
                       0xf << (4*(LED_WHITE_PIN - 8)));

    /* set pins to out */
    LED_PORT->CR[1] |= (0x3 << (4*(LED_RED_PIN - 8)) |
                      0x3 << (4*(LED_GREEN_PIN - 8)) |
                      0x3 << (4*(LED_BLUE_PIN - 8)) |
                      0x3 << (4*(LED_WHITE_PIN - 8)));

    LED_PORT->BSRR = (1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) | (1 << LED_BLUE_PIN) | (1 << LED_WHITE_PIN);
}
