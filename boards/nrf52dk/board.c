/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52dk
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    NRF_P0->DIRSET = (LED1_MASK | LED2_MASK | LED3_MASK | LED4_MASK);
    NRF_P0->OUTSET = (LED1_MASK | LED2_MASK | LED3_MASK | LED4_MASK);

    /* initialize the CPU */
    cpu_init();
}
