/*
 * Copyright (C) 2016 University of California, Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hamilton
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Hamilton mote
 *
 * @author      Michael Andersen <m.andersen@berkeley.edu>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();
    /* initialize the boards LED at pin PA19 */
    LED_PORT.DIRSET.reg = (1 << LED_PIN);
    LED_PORT.OUTCLR.reg = (1 << LED_PIN);
    LED_PORT.PINCFG[LED_PIN].bit.PULLEN = false;
}
