/**
 * Shell commands for temperature and humidity sensor 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_sht11.c
 * @brief   provides shell commands to poll sht11 sensor 
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sht11.h>
#include <string.h>

#ifdef MODULE_SHT11

extern float sht11_temperature_offset;

void _get_humidity_handler(char *unused)
{
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY | TEMPERATURE);

    if(!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%%\n",
               (double) sht11_val.relhum, (double) sht11_val.relhum_temp);
    }
}
void _get_temperature_handler(char *unused)
{
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, TEMPERATURE);

    if(!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Temperature: %-6.2f°C\n", (double) sht11_val.temperature);
    }
}
void _get_weather_handler(char *unused)
{
    uint8_t success;
    sht11_val_t sht11_val;
    success = sht11_read_sensor(&sht11_val, HUMIDITY | TEMPERATURE);

    if(!success) {
        printf("Error reading SHT11\n");
    }
    else {
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%% ",
               (double) sht11_val.relhum, (double) sht11_val.relhum_temp);
        printf("Temperature: %-6.2f°C\n", (double) sht11_val.temperature);
    }
}

void _set_offset_handler(char *offset)
{
    if(strlen(offset) == 6) {
        puts("Usage: offset <OFFSET>");
    }
    else {
        sht11_temperature_offset = atoi(offset + 7);
        printf("Temperature offset set to %f\n", (double) sht11_temperature_offset);
    }
}

#endif
