/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_e104-bt5010a-tb
 * @{
 *
 * @file
 * @brief       Board initialization for the E104-BT5010A Test Board
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize the boards LEDs */
    LED_PORT->DIRSET = (LED_MASK);
    LED_PORT->OUTSET = (LED_MASK);

    /* initialize the CPU */
    cpu_init();
}
