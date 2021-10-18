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
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the on-board LED */
    gpio_init(LED0_PIN, GPIO_OUT);

}
