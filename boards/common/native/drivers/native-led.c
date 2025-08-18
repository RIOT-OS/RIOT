/*
 * SPDX-FileCopyrightText: 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * Native Board LED implementation
 *
 * Only prints function calls at the moment.
 *
 * @ingroup boards_common_native
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
