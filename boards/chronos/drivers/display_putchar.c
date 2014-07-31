/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos putchar via display implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Aleksandr Mikoff <sir.enmity@gmail.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "display.h"

extern void (*_putchar)(int c);

static char display_buf[11];

void putchar_to_display(int c);

void init_display_putchar(void)
{
    memset(display_buf, '\0', 11);
    _putchar = putchar_to_display;
}

void putchar_to_display(int c)
{
    if (c == '\n') {
        display_buf[4] = 1;
        return;
    }

    if (display_buf[4]) {
        memset(display_buf, '\0', 11);
    }
    else {
        display_buf[0] = display_buf[1];
        display_buf[1] = display_buf[2];
        display_buf[2] = display_buf[3];
        display_buf[3] = display_buf[5];
        display_buf[5] = display_buf[6];
        display_buf[6] = display_buf[7];
        display_buf[7] = display_buf[8];
        display_buf[8] = display_buf[9];
    }

    display_buf[9] = toupper(c);

    clear_display_all();

    display_chars(LCD_SEG_L1_3_0, display_buf, SEG_ON);
    display_chars(LCD_SEG_L2_5_0, display_buf + 4, SEG_ON);
}
