/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_pca10001
 * @{
 *
 * @file
 * @brief       Board initialization code for the Nordic PCA10001 board
 *
 * @author      Attilio Dona'
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs: set pins to output and turn LEDs off */
    NRF_GPIO->DIRSET = (LED_RED_PIN | LED_GREEN_PIN);

    NRF_GPIO->OUTCLR = (LED_RED_PIN | LED_GREEN_PIN);

    /* trigger the CPU initialization code */
    cpu_init();
}
