/*
 * Copyright (C) 2017 RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc1
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Sensirion SHTC1 temperature
 *              and humidity sensor.
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "shtc1.h"
#include "shtc1_regs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "assert.h"

/*
 * does a crc check and returns 0 for passed and -1 for failed
 * 2 bytes will be checked for the checksum, rec_value is pointer
 * to first value
 */
static int8_t _check_crc(uint8_t *rec_values, uint8_t right_crc)
{
    uint8_t crc = 0xFF;
    uint8_t current_byte;
    uint8_t bit;

    for (current_byte = 0; current_byte < 2; current_byte++) {
        crc ^= (rec_values[current_byte]);
        for (bit = 8; bit > 0; bit--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ SHTC1_CRC;
            }
            else {
                crc = (crc << 1);
            }
        }
    }
    return !(crc == right_crc);
}

int8_t shtc1_init(shtc1_t *const dev, const shtc1_params_t *params)
{
    /* check for a valid device descriptor and parameters */
    assert(dev && params);
    /* copy settings into the device descriptor */
    dev->params = *params;
    /* Verify the connection by reading the SHTC1's ID and checking its value */
    if (shtc1_id(dev) != SHTC1_OK || ((dev->values.id & 0x3F) != SHTC1_ID)) {
        return SHTC1_ERROR;
    }
    return SHTC1_OK;
}

int8_t shtc1_measure(shtc1_t *const dev)
{
    /* Build and issue the measurement command */
    uint8_t cmd[] = { SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_HIGH, SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_LOW };

    i2c_acquire(dev->params.bus);
    if (i2c_write_bytes(dev->params.bus, dev->params.addr, cmd, 2, 0)) {
        return SHTC1_ERROR;
    }
    /* Receive the measurement */
    /* 16 bit Temperature
     * 8 bit  CRC temp
     * 16 Bit Absolute Humidity
     * 8 bit CRC Hum
     */
    uint8_t received[6];
    if (i2c_read_bytes(dev->params.bus, dev->params.addr, received, 6, 0)) {
        return SHTC1_ERROR;
    }
    i2c_release(dev->params.bus);
    /* get 16bit values and check crc */
    uint16_t temp_f = ((received[0] << 8) | received[1]);
    uint16_t abs_humidity = ((received[3] << 8) | received[4]);
    if (dev->params.crc) {
        if (!((_check_crc(&received[0], received[2]) == 0) && (_check_crc(&received[3], received[5]) == 0))) {
            /* crc check failed */
            DEBUG("CRC Error");
            return SHTC1_ERROR;
        }
        DEBUG("CRC Passed! \n");
    }
    /* calculate the relative humidity and convert the temperature to centi °C */
    /* dev->values.temp = (175.0 * 100 * temp_f / 65536) - 45 ; */
    dev->values.temp =  ((17500 * (uint32_t)temp_f ) >>16 ) - 4500;
    /* dev->values.rel_humidity = 10000 * ( abs_humidity /65536) */
    dev->values.rel_humidity = (10000 * (uint32_t)abs_humidity) >> 16;

    return SHTC1_OK;
}

int8_t shtc1_id(shtc1_t *const dev)
{
    /* Build and send measurement command */
    uint8_t data[] = { SHTC1_COMMAND_ID_HIGH, SHTC1_COMMAND_ID_LOW };
    int8_t check = 0;

    i2c_acquire(dev->params.bus);
    check = i2c_write_bytes(dev->params.bus, dev->params.addr, data, 2, 0);
    /* receive ID and check if the send and receive commands were successfull */
    check += i2c_read_bytes(dev->params.bus, dev->params.addr, data, 2, 0);
    i2c_release(dev->params.bus);
    if (check != 0) {
        /* error occured */
        return SHTC1_ERROR;
    }
    /* Save ID in device descriptor */
    dev->values.id = (data[0] << 8) | data[1];
    return SHTC1_OK;
}

int8_t shtc1_reset(const shtc1_t *const dev)
{
    /* Build and issue the reset command */
    uint8_t data[] = { SHTC1_COMMAND_RESET_HIGH, SHTC1_COMMAND_RESET_LOW };

    i2c_acquire(dev->params.bus);
    if (i2c_write_bytes(dev->params.bus, dev->params.addr, data, 2, 0)) {
        i2c_release(dev->params.bus);
        return SHTC1_ERROR;
    }
    i2c_release(dev->params.bus);
    return SHTC1_OK;
}
