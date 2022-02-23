/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nrf6310
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the NRF6310 board
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void led_init(void)
{
    /* initialize and turn off on-board LEDs */
    NRF_GPIO->DIRSET = LED0_MASK | LED1_MASK | LED2_MASK;
    NRF_GPIO->OUTSET = LED0_MASK | LED1_MASK | LED2_MASK;
}
