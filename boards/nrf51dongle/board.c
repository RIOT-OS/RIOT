/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf51dongle
 * @{
 *
 * @file
 * @brief       Board initialization code for the nRF51 Dongle
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs, set pins as output and turn LEDs off */
    NRF_GPIO->DIRSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
    NRF_GPIO->OUTSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
    /* initialize the CPU */
    cpu_init();
}
