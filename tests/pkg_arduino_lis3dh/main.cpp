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
 * @brief    Application for testing the Adafruit LIS3DH driver package
 *
 * @author   Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "arduino_board.h"

#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

#define LIS3DH_CS 10
#define LIS3DH_I2C_ADDR_1   (0x18)
#define LIS3DH_I2C_ADDR_2   (0x18)

int main(void)
{
#if IS_USED(MODULE_ARDUINO_LIS3DH_SPI)
    Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);       /* SPI */
#else
    Adafruit_LIS3DH lis = Adafruit_LIS3DH();                /* I2C */
#endif

    Serial.print("Initialize LIS3DH ... ");

    if (!lis.begin(LIS3DH_I2C_ADDR_1)) {
        Serial.println("not found");
        return 1;
    }

    Serial.println("ok");

    lis.setRange(LIS3DH_RANGE_4_G);

    Serial.print("Range = ");
    Serial.print(2 << lis.getRange());
    Serial.println("G");

    while (1) {
        /* get X Y and Z data at once */
        lis.read();

        /* Then print out the raw data */
        Serial.print("X: ");
        Serial.print(lis.x);
        Serial.print("\tY: ");
        Serial.print(lis.y);
        Serial.print("\tZ: ");
        Serial.print(lis.z);
        Serial.println();
        delay(200);
    }

    return 0;
}
