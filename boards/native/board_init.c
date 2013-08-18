/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the LGPLv2. See
 * the file LICENSE in the top level directory for more details.
 *
 * @ingroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include "board.h"

#include "board_internal.h"

/**
 * Nothing to initialize at the moment.
 * Turns the red LED on and the green LED off.
 */
void board_init()
{
    LED_GREEN_OFF;
    LED_RED_ON;
    puts("RIOT native board initialized.");
}
