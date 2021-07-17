/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Board initialization for the Waveshare nRF52840 Eval Kit
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    LED0_PORT->DIRSET |= (LED0_MASK);
    LED0_PORT->OUTSET |= (LED0_MASK);
    LED1_PORT->DIRSET |= (LED1_MASK);
    LED1_PORT->OUTSET |= (LED1_MASK);
    LED2_PORT->DIRSET |= (LED2_MASK);
    LED2_PORT->OUTSET |= (LED2_MASK);
    LED3_PORT->DIRSET |= (LED3_MASK);
    LED3_PORT->OUTSET |= (LED3_MASK);

    /* initialize the CPU */
    cpu_init();
}
