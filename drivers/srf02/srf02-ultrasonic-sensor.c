/*
 * srf02-ultrasonic-sensor.c - Driver for the SRF02 ultrasonic.
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
 * @brief       Driver for the SRF02 ultrasonic ranger.
 *              The connection between the MCU and the SRF08 is based on the
 *              i2c-interface.
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @version     $Revision: 3856 $
 *
 * @note        $Id: srf02-ultrasonic-sensor.c 3857 2013-09-24 17:39:33 kasmi $
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "msg.h"
#include "vtimer.h"
#include "timex.h"
#include "thread.h"
#include "hwtimer.h"
#include "srf02-ultrasonic-sensor.h"
#include "i2c.h"

bool srf02_init(uint8_t i2c_interface, uint32_t baud_rate)
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

uint32_t srf02_get_distance(uint8_t ranging_mode)
{
    bool status = false;
    uint8_t reg_size = 1;
    uint8_t range_high_byte = 0;
    uint8_t range_low_byte = 0;
    uint8_t rx_buff[reg_size];
    uint8_t tx_buff[reg_size];
    uint32_t distance = 0;

    tx_buff[0] = ranging_mode;
    status = i2c_write(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
                       SRF02_COMMAND_REG, tx_buff, reg_size);

    if (!status) {
        puts("Write the ranging command to the i2c-interface is failed");
        distance = UINT32_MAX;
        return distance;
    }

    hwtimer_wait(HWTIMER_TICKS(65000));

    status = i2c_read(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
                      SRF02_RANGE_HIGH_BYTE, rx_buff, reg_size);

    if (!status) {
        puts("Read the high echo byte from the i2c-interface is failed");
        distance = UINT32_MAX;
        return distance;
    }

    range_high_byte = rx_buff[0];

    status = i2c_read(SRF02_I2C_INTERFACE, SRF02_DEFAULT_ADDR,
                      SRF02_RANGE_LOW_BYTE, rx_buff, reg_size);

    if (!status) {
        puts("Read the low echo byte from the i2c-interface is failed");
        distance = UINT32_MAX;
        return distance;
    }

    range_low_byte = rx_buff[0];

    distance = (range_high_byte << 8) | range_low_byte;
    //printf("%u | %u\n", range_high_byte, range_low_byte);
    return distance;
}

void srf02_start_ranging(uint16_t ranging_mode)
{

    while (1) {
        uint32_t distance = srf02_get_distance(ranging_mode);

        if (distance != UINT32_MAX) {
            switch (ranging_mode) {
                case SRF02_REAL_RANGING_MODE_CM :
                    printf("distance = %lu cm\n", distance);
                    break;

                case SRF02_REAL_RANGING_MODE_INCH :
                    printf("distance = %lu inch\n", distance);
                    break;

                case SRF02_REAL_RANGING_MODE_MICRO_SEC:
                    // dist_m = 0.000172 distance_micro_sec (air)
                    printf("distance = %lu micro_sec\n", distance);
                    break;

                case SRF02_FAKE_RANGING_MODE_CM:
                case SRF02_FAKE_RANGING_MODE_INCH:
                case SRF02_FAKE_RANGING_MODE_MICRO_SEC:
                    printf("distance fake ranging = %lu \n", distance);
                    break;

                default:
                    printf("distance = %lu cm\n", distance);
            }

            hwtimer_wait(HWTIMER_TICKS(50000));
        }
        else {
            break;
        }
    }

    puts("The SRF02 range sampling is ended!!");
}
