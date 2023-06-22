/*
 * Copyright (C) 2017 HAW Hamburg
 *               2023 Hugues Larrive
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
 * @brief       Application to test i2c on atmega8 using an hacked
 *              hd44780 LCD driver and a pcf8574a i2c I/O expander.
 *
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <stdio.h>

#include "hd44780_avr/hd44780_avr.h"
#include "hd44780_avr/hd44780_avr_params.h"

int main(void)
{
    hd44780_t dev;
    /* init display */
    hd44780_init(&dev, &hd44780_params[0]);
    hd44780_clear(&dev);
    hd44780_home(&dev);
    hd44780_print(&dev, "Test i2c on atmega8");
    hd44780_set_cursor(&dev, 3, 1);
    hd44780_print(&dev, "RIOT is here!");
    hd44780_set_cursor(&dev, 0, 2);
    hd44780_print(&dev, "You can read this so");
    hd44780_set_cursor(&dev, 2, 3);
    hd44780_print(&dev, "i2c is working!");

    return 0;
}
