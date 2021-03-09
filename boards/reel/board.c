/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_reel
 * @{
 *
 * @file
 * @brief       Board initialization for the Phytec 'reel board'
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* setup on-board LEDs */
    NRF_P0->DIRSET = (LED_MASK_P0);
    NRF_P0->OUTSET = (LED_MASK_P0);
    NRF_P1->DIRSET = (LED_MASK_P1);
    NRF_P1->OUTSET = (LED_MASK_P1);

}
