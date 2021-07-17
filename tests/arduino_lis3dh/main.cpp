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
 * @brief    Application for testing the Arduino SPI and I2C (TwoWire) interfaces
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

#ifdef TEST_SPI
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);       /* SPI */
#else
Adafruit_LIS3DH lis = Adafruit_LIS3DH();                /* I2C */
#endif

int main(void)
{
    if (!lis.begin(0x18)) {   /* change this to 0x19 for alternative i2c address */
        Serial.println("Couldnt start");
        return 1;
    }
    lis.setRange(LIS3DH_RANGE_4_G);

    Serial.print("Range = "); Serial.print(2 << lis.getRange());  
    Serial.println("G");

    while (1) {
        lis.read();      /* get X Y and Z data at once */
        /* Then print out the raw data */
        Serial.print("X:  "); Serial.print(lis.x); 
        Serial.print("  \tY:  "); Serial.print(lis.y); 
        Serial.print("  \tZ:  "); Serial.print(lis.z);
        Serial.println();
        delay(200); 
    }

    return 0;
}
