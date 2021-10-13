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
#include <string.h>
#include <math.h>

#include "log.h"
#include "assert.h"
#include "checksum/crc8.h"
#include "shtc1.h"
#include "shtc1_regs.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int8_t shtc1_init(shtc1_t *const dev, const shtc1_params_t *params)
{
    uint16_t id;

    /* check for a valid device descriptor and parameters */
    assert(dev && params);
    /* copy settings into the device descriptor */
    dev->params = *params;
    /* Verify the connection by reading the SHTC1's ID and checking its value */
    if (shtc1_id(dev, &id) != SHTC1_OK) {
        return SHTC1_ERROR_BUS;
    }
    else if ((id & 0x3F) != SHTC1_ID) {
        return SHTC1_ERROR_CRC;
    }
    else {
        return SHTC1_OK;
    }
}

int8_t shtc1_read(const shtc1_t *dev, uint16_t *rel_humidity,
                  int16_t *temperature)
{
    uint8_t received[6];
    /* Build and issue the measurement command */
    uint8_t cmd[] =
    { SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_HIGH,
      SHTC1_MEASURE_CLOCK_STRETCHING_TEMP_LOW };

    i2c_acquire(dev->params.i2c_dev);

    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, cmd, 2, 0)) {
        return SHTC1_ERROR_BUS;
    }
    /* Receive the measurement */
    /* 16 bit Temperature
     * 8 bit  CRC temp
     * 16 Bit Absolute Humidity
     * 8 bit CRC Hum
     */
    if (i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, received, 6,
                       0)) {
        return SHTC1_ERROR_BUS;
    }
    i2c_release(dev->params.i2c_dev);

    if (rel_humidity != NULL) {
        uint32_t abs_humidity = ((received[3] << 8) | received[4]);
        /* 10000 * ( abs_humidity /65536) */
        *rel_humidity = (10000 * abs_humidity) >> 16;
    }

    if (temperature != NULL) {
        uint16_t temp_f = ((received[0] << 8) | received[1]);
        /* calculate the relative humidity and convert the temperature to centi °C */
        /* (175.0 * 100 * temp_f / 65536) - 45 ; */
        *temperature = ((17500 * (uint32_t)temp_f) >> 16) - 4500;
    }

    if (!((crc8(&received[0], 2, SHTC1_CRC, 0xFF) == received[2]) &&
          (crc8(&received[3], 2, SHTC1_CRC, 0xFF) == received[5]))) {
        /* crc check failed */
        DEBUG("CRC Error\n");
        return SHTC1_ERROR_CRC;
    }
    DEBUG("CRC Passed! \n");
    return SHTC1_OK;
}

int8_t shtc1_id(const shtc1_t *dev, uint16_t *id)
{
    /* Build and send measurement command */
    uint8_t data[] = { SHTC1_COMMAND_ID_HIGH, SHTC1_COMMAND_ID_LOW };

    i2c_acquire(dev->params.i2c_dev);
    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, data, 2,
                        0)) {
        return SHTC1_ERROR_BUS;
    }
    /* receive ID and check if the send and receive commands were successful */
    if (i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, data, 2, 0)) {
        return SHTC1_ERROR_BUS;
    }
    i2c_release(dev->params.i2c_dev);

    /* Save ID in device descriptor */
    *id = (data[0] << 8) | data[1];
    return SHTC1_OK;
}

int8_t shtc1_reset(const shtc1_t *const dev)
{
    /* Build and issue the reset command */
    uint8_t data[] = { SHTC1_COMMAND_RESET_HIGH, SHTC1_COMMAND_RESET_LOW };

    i2c_acquire(dev->params.i2c_dev);
    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, data, 2,
                        0)) {
        return SHTC1_ERROR_BUS;
    }
    i2c_release(dev->params.i2c_dev);
    return SHTC1_OK;
}
