/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_weio
 * @{
 *
 * @file
 * @brief       Board specific implementations for the WeIO board
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

void board_init(void)
{
    /* initialize and turn off the boards LEDs */
    LED_PORT->DIR[1] = (LED0_MASK | LED1_MASK | LED2_MASK);
    LED_PORT->SET[1] = (LED0_MASK | LED1_MASK | LED2_MASK);

    /* initialize the CPU */
    cpu_init();
}
