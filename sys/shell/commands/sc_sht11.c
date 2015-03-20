/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to poll sht11 sensor
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sht11.h"

#ifdef MODULE_SHT11

extern float sht11_temperature_offset;

int _get_humidity_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY | TEMPERATURE);

    if (!success) {
        printf("Error reading SHT11\n");

        return 1;
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%%\n",
               (double) sht11_val.relhum, (double) sht11_val.relhum_temp);

        return 0;
    }
}

int _get_temperature_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, TEMPERATURE);

    if (!success) {
        printf("Error reading SHT11\n");

        return 1;
    }
    else {
        printf("Temperature: %-6.2f°C\n", (double) sht11_val.temperature);

        return 0;
    }
}

int _get_weather_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY | TEMPERATURE);

    if (!success) {
        printf("Error reading SHT11\n");

        return 1;
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%% ",
               (double) sht11_val.relhum, (double) sht11_val.relhum_temp);
        printf("Temperature: %-6.2f°C\n", (double) sht11_val.temperature);

        return 0;
    }
}

int _set_offset_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <OFFSET>\n", argv[0]);

        return 1;
    }
    else {
        sht11_temperature_offset = atoi(argv[1]);
        printf("Temperature offset set to %f\n", (double) sht11_temperature_offset);

        return 0;
    }
}

#endif
