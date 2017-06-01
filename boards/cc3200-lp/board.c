/*
 * Copyright (C) 2015 Dinh Nguyen
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
 * @author      Dinh Nguyen <nqdinhddt@gmail.com>
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
    /* enable clocks for GPIOA1 */
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);

    /* configure PIN_64 red as general output */
    MAP_PinTypeGPIO(LED_RED_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

    /* configure PIN_01 orange as general output */
    MAP_PinTypeGPIO(LED_ORANGE_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

    /* configure PIN_02 green as general output */
    MAP_PinTypeGPIO(LED_GREEN_PIN, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);

    /* turn off all leds */
    LED_RED_OFF();
    LED_ORANGE_OFF();
    LED_GREEN_OFF();
}