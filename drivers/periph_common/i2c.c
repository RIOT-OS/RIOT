/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Common I2C fallback implementations
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/i2c.h"

#ifdef PERIPH_I2C_NEEDS_READ_REG
int i2c_read_reg(ic2_t dev, uint16_t addr, uint16_t reg,
                 uint8_t *data, size_t len, uint8_t flags)
{
    int res;

    /* NOSTART invalid for this function */
    flags &= ~(I2C_NOSTART);

    /* write register address */
    if (flags & I2C_REG16) {
        uint8_t r = (addr & 0xff);
        res = i2c_write(dev, addr, &r, 1, (flags | I2C_NOSTOP));
    } else {
        res = i2c_write(dev, addr, &addr, 2, (flags | I2C_NOSTOP));
    }

    /* check for errors */
    if (res != I2C_ACK) {
        return res;
    }

    /* read actual data */
    return i2c_read(dev, addr, data, len, flags);
}
#endif

#ifdef PERIPH_I2C_NEEDS_WRITE_BYTE
int i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags)
{
    return i2c_write(dev, addr, &data, 1, flags);
}
#endif

#ifdef PERIPH_I2C_NEEDS_WRITE_REG
int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  const uint8_t *data, size_t len, uint8_t flags)
{
    int res;

    /* NOSTART invalid for this function */
    flags &= ~(I2C_NOSTART);

    /* write register address */
    if (flags & I2C_REG16) {
        uint8_t r = (addr & 0xff);
        res = i2c_write(dev, addr, &r, 1, (flags | I2C_NOSTOP));
    } else {
        res = i2c_write(dev, addr, &addr, 2, (flags | I2C_NOSTOP));
    }

    /* check for errors */
    if (res != I2C_ACK) {
        return res;
    }

    /* write data */
    return i2c_write(dev, addr, data, len, flags);
}
#endif
