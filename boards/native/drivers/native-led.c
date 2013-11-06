/**
 * Native Board LED implementation
 *
 * Only prints function calls at the moment.
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

void _native_LED_GREEN_OFF(void)
{
    printf("LED_GREEN_OFF\n");
}

void _native_LED_GREEN_ON(void)
{
    printf("LED_GREEN_ON\n");
}

void _native_LED_GREEN_TOGGLE(void)
{
    printf("LED_GREEN_TOGGLE\n");
}

void _native_LED_RED_OFF(void)
{
    printf("LED_RED_OFF\n");
}

void _native_LED_RED_ON(void)
{
    printf("LED_RED_ON\n");
}

void _native_LED_RED_TOGGLE(void)
{
    printf("LED_RED_TOGGLE\n");
}

