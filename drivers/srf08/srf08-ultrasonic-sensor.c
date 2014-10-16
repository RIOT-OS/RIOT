/*
 * srf08-ultrasonic-sensor.c - Driver for the SRF08 ultrasonic ranger via the
 * i2c interface.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @internal
 * @brief       Driver for the SRF08 ultrasonic ranger using the i2c interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3857 $
 *
 * @note        $Id: srf08-ultrasonic-sensor.c 3857 2013-09-05 14:45:13 kasmi $
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "hwtimer.h"
#include "srf08-ultrasonic-sensor.h"
#include "i2c.h"

bool srf08_init(uint8_t i2c_interface, uint32_t baud_rate)
{
    if (i2c_initialize(i2c_interface, (uint32_t) I2CMASTER, 0, baud_rate, NULL)
        == false) { /* initialize I2C */
        puts("fatal error happened in i2c_initialize()\n");
        return false;
    }
    else {
        i2c_enable_pull_up_resistor(i2c_interface);
        //i2c_disable_pull_up_resistor(i2c_interface);
        return true;
    }
}

void srf08_set_range(uint8_t max_range)
{
    uint8_t tx_buff[1];

    tx_buff[0] = max_range;
    i2c_write(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR, SRF08_RANGE_REG, tx_buff,
              1);
}

void srf08_set_gain(uint8_t gain)
{
    uint8_t tx_buff[1];

    tx_buff[0] = gain;
    i2c_write(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR, SRF08_GAIN_REG, tx_buff,
              1);
}

uint8_t srf08_get_range(void)
{
    uint8_t rx_buff[1];

    i2c_read(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR, SRF08_RANGE_REG, rx_buff,
             1);
    return rx_buff[0];
}

uint8_t srf08_get_gain(void)
{
    uint8_t rx_buff[1];

    i2c_read(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR, SRF08_GAIN_REG, rx_buff,
             1);
    return rx_buff[0];
}


int32_t srf08_get_distances(uint32_t *range_array, uint8_t ranging_mode)
{
    bool status;
    uint8_t reg_size = 1;
    uint8_t rx_buff[reg_size];
    uint8_t tx_buff[reg_size];
    uint8_t register_location;
    uint8_t echo_number = 0;

    tx_buff[0] = ranging_mode;
    status = i2c_write(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR,
                       SRF08_COMMAND_REG, tx_buff, reg_size);
    if (!status) {
        puts("Write the ranging command to the i2c-interface is failed");
        return -1;
    }
    hwtimer_wait(HWTIMER_TICKS(70000));

    // Read all echo buffers
    for (register_location = 2; register_location < SRF08_MAX_REGISTER_NUMBER;
         register_location += 2) {
        //read the high echo byte
        status = i2c_read(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR,
                          register_location, rx_buff, reg_size);
        if (!status) {
            puts("Read the high echo byte from the i2c-interface is failed");
            return -1;
        }
        uint8_t range_high_byte = rx_buff[0];
        //read the low echo byte
        status = i2c_read(SRF08_I2C_INTERFACE, SRF08_DEFAULT_ADDR,
                          register_location + 1, rx_buff, reg_size);
        if (!status) {
            puts("Read the low echo byte from the i2c-interface is failed");
            return -1;
        }
        uint8_t range_low_byte = rx_buff[0];
        if ((range_high_byte == 0) && (range_low_byte == 0)) {
            break;
        }
        else {
            uint32_t distance = (range_high_byte << 8) | range_low_byte;
            range_array[(register_location - 2) / 2] = distance;
            echo_number++;
            printf("distance = %4lu cm , echo%d\n",
                   distance, register_location / 2);
        }
        hwtimer_wait(HWTIMER_TICKS(500000));
    }
    puts("--------------------------------------------");
    return echo_number;
}

void srf08_start_ranging(uint8_t ranging_mode)
{
    uint32_t range_array[SRF08_MAX_ECHO_NUMBER];
    uint8_t i;

    puts("Ultrasonic SRF08 engine is started");
    //wait due to calibration
    hwtimer_wait(HWTIMER_TICKS(700000));
    printf("Actual range = %d\n", srf08_get_range());
    printf("Actual gain = %d\n", srf08_get_gain());

    while (1) {
        puts("--------------------------------------------");
        uint8_t echo_number = srf08_get_distances(range_array, ranging_mode);
        if (echo_number > 0) {
            for (i = 0; i < echo_number; i++) {
                printf("stored distance = %4lu cm , echo%d\n", range_array[i], i + 1);
            }
        }
        else {
            break;
        }
    }
}
