/*
 * Copyright (C) 2021 Gunar Schorcht
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
 * @brief    Test application for Arduino Adafruit LSM9DS0 driver
 *
 * @author   Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "arduino_board.h"

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>

#define LSM9DS0_XM_CS   10  /* Accelerator/Magnetometer CS */
#define LSM9DS0_G_CS    9   /* Gyroscope CS */

int main(void)
{
     /* sensor base ID = 1000 */
#if IS_USED(MODULE_ARDUINO_ADAFRUIT_LSM9DS0_SPI)
    Adafruit_LSM9DS0 sensor(LSM9DS0_XM_CS, LSM9DS0_G_CS, 1000);
#else
    Adafruit_LSM9DS0 sensor(1000);
#endif

    Serial.print("Initialize LSM9DS0 ... ");

    if(!sensor.begin()) {
        Serial.print("not found");
        return 1;
    }
    Serial.println("ok");

    sensor.getAccel().printSensorDetails();
    sensor.getMag().printSensorDetails();
    sensor.getGyro().printSensorDetails();
    sensor.getTemp().printSensorDetails();

    sensor.setupAccel(sensor.LSM9DS0_ACCELRANGE_2G);
    sensor.setupMag(sensor.LSM9DS0_MAGGAIN_2GAUSS);
    sensor.setupGyro(sensor.LSM9DS0_GYROSCALE_245DPS);


    while (1) {
        sensors_event_t e_accel, e_mag, e_gyro, e_temp;

        sensor.getEvent(&e_accel, &e_mag, &e_gyro, &e_temp);

        Serial.print("Accel\tX: ");
        Serial.print(e_accel.acceleration.x);
        Serial.print("   \tY: ");
        Serial.print(e_accel.acceleration.y);
        Serial.print("   \tZ: ");
        Serial.print(e_accel.acceleration.z);
        Serial.println("   \tm/s^2");

        Serial.print("Magn.\tX: ");
        Serial.print(e_mag.magnetic.x);
        Serial.print("   \tY: ");
        Serial.print(e_mag.magnetic.y);
        Serial.print("   \tZ: ");
        Serial.print(e_mag.magnetic.z);
        Serial.println("   \tuT");

        Serial.print("Gyro\tX: ");
        Serial.print(e_gyro.gyro.x);
        Serial.print("   \tY: ");
        Serial.print(e_gyro.gyro.y);
        Serial.print("   \tZ: ");
        Serial.print(e_gyro.gyro.z);
        Serial.println("   \trad/s");

        Serial.print("Temp: ");
        Serial.print(e_temp.temperature);
        Serial.println("   \t°C");

        Serial.println("**********************\n");

        delay(250);
    }
    return 0;
}
