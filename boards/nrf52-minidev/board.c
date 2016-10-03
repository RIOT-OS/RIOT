/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Frank Holtz <frank-riot2015@holtznet.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52-minidev
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF52 mini development
 *              board. https://github.com/RIOT-OS/RIOT/wiki/Board:-nRF52-minidev
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    NRF_P0->DIRSET = (LED0_MASK | LED1_MASK );
    NRF_P0->OUTSET = (LED0_MASK | LED1_MASK );
    NRF_P0->OUTSET = (LED0_MASK | LED1_MASK );

    /* initialize the CPU */
    cpu_init();
}
