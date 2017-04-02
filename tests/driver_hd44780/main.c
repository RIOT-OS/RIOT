/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for Arduino LCM1602C LCD
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "hd44780.h"
#include "hd44780_params.h"

int main(void)
{
    hd44780_t dev;
    /* init display */
    puts("[START]");
    if (hd44780_init(&dev, &hd44780_params[0]) != 0) {
        puts("[FAILED]");
        return 1;
    }
    /* clear screen, reset cursor */
    hd44780_clear(&dev);
    hd44780_home(&dev);
    /* write first line */
    hd44780_print(&dev, "Hello World ...");
    xtimer_sleep(1);
    /* set cursor to second line and write */
    hd44780_set_cursor(&dev, 0, 1);
    hd44780_print(&dev, "   RIOT is here!");
    xtimer_sleep(3);
    /* clear screen, reset cursor */
    hd44780_clear(&dev);
    hd44780_home(&dev);
    /* write first line */
    hd44780_print(&dev, "The friendly IoT");
    /* set cursor to second line and write */
    hd44780_set_cursor(&dev, 0, 1);
    hd44780_print(&dev, "Operating System");

    puts("[SUCCESS]");

    return 0;
}
