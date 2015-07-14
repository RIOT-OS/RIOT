/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_pca10000
 * @{
 *
 * @file
 * @brief       Board initialization code for the Nordic PCA10000 board
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs: set pins to output and turn LEDs off */
    NRF_GPIO->DIRSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
    NRF_GPIO->OUTSET = (LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN);
    /* trigger the CPU initialization code */
    cpu_init();
}
