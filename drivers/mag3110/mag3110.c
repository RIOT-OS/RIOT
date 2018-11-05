/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mag3110
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MAG3110 magnetometer.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "periph/i2c.h"

#include "mag3110.h"
#include "mag3110_reg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)

int mag3110_init(mag3110_t *dev, const mag3110_params_t *params)
{
    uint8_t reg;

    assert(dev);
    assert(params);

    /* write device descriptor */
    memcpy(dev, params, sizeof(mag3110_params_t));

    i2c_acquire(BUS);
    /* test device */
    i2c_read_regs(BUS, ADDR, MAG3110_WHO_AM_I, &reg, 1, 0);
    if (reg != dev->params.type) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_init: invalid WHO_AM_I value (0x%02x)!\n", (int)reg);
        return -MAG3110_ERROR_DEV;
    }
    /* enable automatic magnetic sensor reset */
    reg = MAG3110_CTRL_REG2_AUTO_MRST_EN;
    if (i2c_write_regs(BUS, ADDR, MAG3110_CTRL_REG2, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_init: failed to enable auto reset!\n");
        return -MAG3110_ERROR_CNF;
    }
    /* set sample rate */
    reg = MAG3110_CTRL_REG1_DROS(dev->params.dros);
    if (i2c_write_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_init: failed to set sample rate!\n");
        return -MAG3110_ERROR_CNF;
    }
    /* set device active */
    if (i2c_read_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_init: failed to read device state!\n");
        return -MAG3110_ERROR_I2C;
    }
    reg |= MAG3110_CTRL_REG1_AC;
    if (i2c_write_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_init: failed to set device active!\n");
        return -MAG3110_ERROR_CNF;
    }
    i2c_release(BUS);
    /* write user offsets */
    return mag3110_set_user_offset(dev, dev->params.offset[0],
                                   dev->params.offset[1], dev->params.offset[2]);
}

int mag3110_set_user_offset(const mag3110_t *dev, int16_t x, int16_t y, int16_t z)
{
    uint8_t buf[6];

    assert(dev);

    buf[0] = (x >> 8);
    buf[1] = x;
    buf[2] = (y >> 8);
    buf[3] = y;
    buf[4] = (z >> 8);
    buf[5] = z;

    DEBUG("[mag3110] setting user offset to X: %3i, Y: %3i, Z: %3i\n",
          (int)x, (int)y, (int)z);

    i2c_acquire(BUS);
    if (i2c_write_regs(BUS, ADDR, MAG3110_OFF_X_MSB, buf, 6, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mag3110_set_user_offset: failed to set offsets!\n");
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);

    return MAG3110_OK;
}

int mag3110_set_active(const mag3110_t *dev)
{
    uint8_t reg;

    assert(dev);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }

    reg |= MAG3110_CTRL_REG1_AC;

    if (i2c_write_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);

    return MAG3110_OK;
}

int mag3110_set_standby(const mag3110_t *dev)
{
    uint8_t reg;

    assert(dev);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }

    reg &= ~MAG3110_CTRL_REG1_AC;

    if (i2c_write_regs(BUS, ADDR, MAG3110_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);

    return MAG3110_OK;
}

int mag3110_is_ready(const mag3110_t *dev)
{
    uint8_t reg;

    assert(dev);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MAG3110_DR_STATUS, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);

    return (int)(reg & MAG3110_DR_STATUS_ZYXDR);
}

int mag3110_read(const mag3110_t *dev, mag3110_data_t *data)
{
    uint8_t buf[7];

    assert(dev);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MAG3110_DR_STATUS, buf, 7, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);
    /* TODO: implement state handling, if needed?
    uint8_t status = buf[0];
    */
    data->x = ((int16_t)buf[1] << 8) | buf[2];
    data->y = ((int16_t)buf[3] << 8) | buf[4];
    data->z = ((int16_t)buf[5] << 8) | buf[6];

    return MAG3110_OK;
}

int mag3110_read_dtemp(const mag3110_t *dev, int8_t *dtemp)
{
    assert(dev);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MAG3110_DIE_TEMP, dtemp, 1, 0) < 0) {
        i2c_release(BUS);
        return -MAG3110_ERROR_I2C;
    }
    i2c_release(BUS);

    return MAG3110_OK;
}
