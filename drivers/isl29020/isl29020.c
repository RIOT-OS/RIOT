/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_isl29020
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

int isl29020_init(isl29020_t *dev, i2c_t i2c, uint8_t address,
                  isl29020_range_t range, isl29020_mode_t mode)
{
    int res;
    uint8_t tmp[] = {ISL29020_REG_CMD, 0};

    /* initialize device descriptor */
    dev->i2c = i2c;
    dev->address = address;
    dev->lux_fac = (float)((1 << (10 + (2 * range))) - 1) / 0xffff;

    /* initialize the I2C bus -> TODO: move to auto-init? */
    if (i2c_init_master(i2c, I2C_SPEED_NORMAL) < 0) {
        DEBUG("[isl29020] error when initializing the I2C bus\n");
    }

    /* configure and enable the sensor */
    tmp[1] = (ISL29020_CMD_EN | ISL29020_CMD_MODE |
              ISL29020_RES_INT_16 | range | (mode << 5));
    i2c_acquire(dev_i2c);
    res = i2c_write(dev->i2c, address, tmp, 2, 0);
    i2c_release(dev->i2c);

    /* make sure the write was successful */
    if (res != I2C_ACK) {
        DEBUG("[isl29020] error while enabling the sensor\n");
        return -1;
    }

    return 0;
}

int isl29020_read(isl29020_t *dev)
{
    uint8_t low, high;
    uint16_t res;

    /* read lighting value */
    i2c_acquire(dev->i2c);
    i2c_write_byte(dev->i2c, &dev->address, ISL29020_REG_LDATA, I2C_NOSTOP);
    i2c_read(dev->i2c, dev->address, &low, 1, 0);
    i2c_write_byte(dev->i2c, &dev->address, ISL29020_REG_LDATA, I2C_NOSTOP);
    i2c_read(dev->i2c, dev->address, &high, 1, 0);
    i2c_release(dev->i2c);

    res = (high << 8) | low;
    DEBUG("ISL29020: Raw value: %i - high: %i, low: %i\n", res, high, low);

    /* calculate and return the actual lux value */
    return (int)(dev->lux_fac * res);
}

int isl29020_enable(isl29020_t *dev)
{
    uint8_t tmp[] = {ISL29020_REG_CMD, 0};

    i2c_acquire(dev->i2c);
    i2c_write_byte(dev->i2c, &dev->address, tmp[0], I2C_NOSTOP);
    i2c_read(dev->i2c, dev->address, &tmp[1], 1, 0);
    tmp[1] |= ISL29020_CMD_EN;
    i2c_write(dev->i2c, &dev->address, tmp, 2, 0);
    i2c_release(dev->i2c);

    return 0;
}

int isl29020_disable(isl29020_t *dev)
{
    uint8_t tmp[] = {ISL29020_REG_CMD, 0};

    i2c_acquire(dev->i2c);
    i2c_write_byte(dev->i2c, &dev->address, tmp[0], I2C_NOSTOP);
    i2c_read(dev->i2c, dev->address, &tmp[1], 1, 0);
    tmp[1] &= ~(ISL29020_CMD_EN);
    i2c_write(dev->i2c, &dev->address, tmp, 2, 0);
    i2c_release(dev->i2c);

    return 0;
}
