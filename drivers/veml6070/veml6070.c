/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_veml6070
 * @{
 *
 * @file
 * @brief       Device driver implementation for the VEML6070 UV sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <math.h>

#include "log.h"
#include "veml6070.h"
#include "veml6070_params.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define VEML6070_ADDRH      (0x39)
#define VEML6070_ADDRL      (0x38)

/*---------------------------------------------------------------------------*
 *                          VEML6070 Core API                                *
 *---------------------------------------------------------------------------*/

int veml6070_init(veml6070_t *dev, const veml6070_params_t * params)
{
    dev->params = *params;

    /* Acquire exclusive access */
    i2c_acquire(dev->params.i2c_dev);

    i2c_write_byte(dev->params.i2c_dev, VEML6070_ADDRL,
                   (uint8_t)(dev->params.itime << 2) | 0x02, 0);

    /* Release I2C device */
    i2c_release(dev->params.i2c_dev);

    return VEML6070_OK;
}

uint16_t veml6070_read_uv(const veml6070_t *dev)
{
    /* Acquire exclusive access */
    i2c_acquire(dev->params.i2c_dev);

    uint8_t buffer[2];
    i2c_read_byte(dev->params.i2c_dev, VEML6070_ADDRL, &buffer[0], 0);
    i2c_read_byte(dev->params.i2c_dev, VEML6070_ADDRH, &buffer[1], 0);

    uint16_t uv = (uint16_t)(buffer[1] << 8) | buffer[0];

    /* Release I2C device */
    i2c_release(dev->params.i2c_dev);

    return uv;
}
