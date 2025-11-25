/*
 * Copyright (C) 2021 Gunar Schorcht <gunar@schorcht.net>
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
 * @brief    Tests the Adafruit Unified Sensor Drive
 *
 * @author   Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>

#include "Adafruit_Sensor.h"

class ADXL345 : public Adafruit_Sensor
{
    public:
        bool getEvent(sensors_event_t *) { return false; };
        void getSensor(sensor_t *sensor);
};

void ADXL345::getSensor(sensor_t *s)
{
    memset(s, 0, sizeof(sensor_t));
    strncpy(s->name, "ADXL345", ARRAY_SIZE(s->name) - 1);
    s->version = 1;
    s->sensor_id = 12345;
    s->type = SENSOR_TYPE_ACCELEROMETER;
    s->min_delay = 0;
    s->min_value = -156.9064F; /* -16g = 156.9064 m/s^2  */
    s->max_value = 156.9064F;  /*  16g = 156.9064 m/s^2  */
    s->resolution = 0.03923F;  /*  4mg = 0.0392266 m/s^2 */
}

int main(void)
{
    ADXL345 adxl345;
    adxl345.printSensorDetails();
}
