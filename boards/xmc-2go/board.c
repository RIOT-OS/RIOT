/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_xmc2go
 * @{
 *
 * @file
 * @brief       Board specific initialization
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 * @}
 */

#include "board.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* set pin 1.0 and 1.1 (LEDs) to work as push-pull general purpose
     * output */
    PORT1->IOCR0 = (1 << 15) | (1 << 7);

    /* select digital input/output on all pins of port 2. An ADC
       driver would have to selectively reverse this. P2.2 - P2.9 are
       input only. */
    PORT2->PDISC = 0UL;
}
