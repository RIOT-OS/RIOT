/*
 * Copyright (C) 2021 J. David Ibáñez <jdavid.ibp@gmail.com>
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
 * @brief    Tests the Arduino-SDI-12 package
 *
 * @author   J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * @}
 */

#include <SDI12.h>

#ifndef SDI12_DATA_PIN
#define SDI12_DATA_PIN 13
#endif

SDI12 sdi12(SDI12_DATA_PIN);

int main(void)
{
    sdi12.begin();
    delay(500);  // allow things to settle

    while (1) {
        sdi12.sendCommand("?I!");
        delay(300);                  // wait a while for a response
        while (sdi12.available())    // write the response to the screen
            Serial.write(sdi12.read());

        // print again in three seconds
        delay(3000);
    }
}
