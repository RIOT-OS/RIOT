/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
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
#ifdef MODULE_UART0
    _native_init_uart0();
#endif
    LED_GREEN_OFF;
    LED_RED_ON;
    puts("RIOT native board initialized.");
}
