/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup native
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include "board.h"

void board_init()
{
    LED_GREEN_OFF();
    LED_RED_ON();
    puts("RIOT native board initialized.");
}
