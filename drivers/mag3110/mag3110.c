/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include "mutex.h"
#include "periph/i2c.h"
#include "mag3110.h"
#include "mag3110_reg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C_SPEED                  I2C_SPEED_FAST

int mag3110_test(mag3110_t *dev)
{
    char reg;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_WHO_AM_I, &reg, 1) != 1) {
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    if (reg != MAG3110_ID) {
        return -1;
    }

    return 0;
}

int mag3110_init(mag3110_t *dev, i2c_t i2c, uint8_t address, uint8_t dros)
{
    char reg;

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    if (dros > MAG3110_DROS_0008_128) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        i2c_release(dev->i2c);
        return -2;
    }
    i2c_release(dev->i2c);

    if (mag3110_test(dev)) {
        return -3;
    }

    /* enable automatic magnetic sensor reset */
    reg = MAG3110_CTRL_REG2_AUTO_MRST_EN;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG2, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }

    reg = MAG3110_CTRL_REG1_DROS(dros);

    if (i2c_write_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }
    i2c_release(dev->i2c);

    dev->initialized = true;

    return 0;
}

int mag3110_set_user_offset(mag3110_t *dev, int16_t x, int16_t y, int16_t z)
{
    char buf[6];

    buf[0] = (char)(x >> 8);
    buf[1] = (char)x;
    buf[2] = (char)(y >> 8);
    buf[3] = (char)y;
    buf[4] = (char)(z >> 8);
    buf[5] = (char)z;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MAG3110_OFF_X_MSB, buf, 6) != 6) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mag3110_set_active(mag3110_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg |= MAG3110_CTRL_REG1_AC;

    if (i2c_write_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mag3110_set_standby(mag3110_t *dev)
{
    char reg;

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg &= ~MAG3110_CTRL_REG1_AC;

    if (i2c_write_regs(dev->i2c, dev->addr, MAG3110_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mag3110_is_ready(mag3110_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_DR_STATUS, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return (int)(reg & MAG3110_DR_STATUS_ZYXDR);
}

int mag3110_read(mag3110_t *dev, int16_t *x, int16_t *y, int16_t *z, uint8_t *status)
{
    char buf[7];

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_DR_STATUS, buf, 7) != 7) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    *status = buf[0];
    *x = ((int16_t)buf[1] << 8) | buf[2];
    *y = ((int16_t)buf[3] << 8) | buf[4];
    *z = ((int16_t)buf[5] << 8) | buf[6];

    return 0;
}

int mag3110_read_dtemp(mag3110_t *dev, int8_t *dtemp)
{
    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MAG3110_DIE_TEMP, (char *)dtemp, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}
