/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief    Demonstrates the use of an Arduino library imported as package
 *
 * @author   Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdint.h>

#include "arduino_board.h"
#include "TalkingLED.h"

#ifndef ARDUINO_LED
#define ARDUINO_LED (13)    /* Arduino Uno LED pin */
#endif

TalkingLED tled;

int main(void)
{
    tled.begin(ARDUINO_LED);

    while (1) {
        /* message 2: short short */
        tled.message(2);
        tled.waitEnd();
        /* message 8: long long */
        tled.message(8);
        tled.waitEnd();
    }

    return 0;
}
