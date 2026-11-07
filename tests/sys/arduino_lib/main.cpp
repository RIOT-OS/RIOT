/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
