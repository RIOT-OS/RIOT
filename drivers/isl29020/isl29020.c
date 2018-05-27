/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_isl29020
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "isl29020.h"
#include "isl29020-internal.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c)
#define DEV_ADDR     (dev->params.addr)
#define DEV_RANGE    (dev->params.range)
#define DEV_MODE     (dev->params.mode)

int isl29020_init(isl29020_t *dev, const isl29020_params_t *params)
{
    dev->params = *params;

    int res;
    uint8_t tmp;

    /* initialize device descriptor */
    dev->lux_fac = (float)((1 << (10 + (2 * DEV_RANGE))) - 1) / 0xffff;

    /* acquire exclusive access to the bus */
    i2c_acquire(DEV_I2C);

    /* configure and enable the sensor */
    tmp = (ISL29020_CMD_EN | ISL29020_CMD_MODE |
           ISL29020_RES_INT_16 | DEV_RANGE | (DEV_MODE << 5));
    res = i2c_write_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_CMD, tmp, 0);
    /* release the bus for other threads */
    i2c_release(DEV_I2C);
    if (res < 0) {
        return -1;
    }
    return 0;
}

int isl29020_read(const isl29020_t *dev)
{
    uint8_t low, high;
    uint16_t res;
    int ret;

    i2c_acquire(DEV_I2C);
    /* read lighting value */
    ret = i2c_read_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_LDATA, &low, 0);
    ret += i2c_read_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_HDATA, &high, 0);
    i2c_release(DEV_I2C);
    if (ret < 0) {
        return -1;
    }
    res = (high << 8) | low;
    DEBUG("ISL29020: Raw value: %i - high: %i, low: %i\n", res, high, low);
    /* calculate and return the actual lux value */
    return (int)(dev->lux_fac * res);
}

int isl29020_enable(const isl29020_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    res = i2c_read_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_CMD, &tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    tmp |= ISL29020_CMD_EN;
    res = i2c_write_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_CMD, tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    i2c_release(DEV_I2C);
    return 0;
}

int isl29020_disable(const isl29020_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    res = i2c_read_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_CMD, &tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    tmp &= ~(ISL29020_CMD_EN);
    res = i2c_write_reg(DEV_I2C, DEV_ADDR, ISL29020_REG_CMD, tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    i2c_release(DEV_I2C);
    return 0;
}
