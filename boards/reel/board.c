/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
