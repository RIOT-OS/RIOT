/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mma8652
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MMA8652 accelerometer.
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
#include "mma8652.h"
#include "mma8652_reg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C_SPEED                  I2C_SPEED_FAST

int mma8652_test(mma8652_t *dev)
{
    char reg;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MMA8652_WHO_AM_I, &reg, 1) != 1) {
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    if (reg != MMA8652_ID) {
        return -1;
    }

    return 0;
}

int mma8652_init(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr, uint8_t range)
{
    char reg;

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    if (dr > MMA8652_DATARATE_1HZ56 || range > MMA8652_FS_RANGE_8G) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        i2c_release(dev->i2c);
        return -2;
    }
    i2c_release(dev->i2c);

    if (mma8652_test(dev)) {
        return -3;
    }

    reg = MMA8652_XYZ_DATA_CFG_FS(range);

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_XYZ_DATA_CFG, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }

    reg = MMA8652_CTRL_REG1_DR(dr);

    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }
    i2c_release(dev->i2c);

    dev->initialized = true;

    return 0;
}

int mma8652_set_user_offset(mma8652_t *dev, int8_t x, int8_t y, int8_t z)
{
    char buf[3];

    buf[0] = (char)x;
    buf[1] = (char)y;
    buf[2] = (char)z;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_OFF_X, buf, 3) != 3) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mma8652_reset(mma8652_t *dev)
{
    char reg;

    dev->initialized = false;
    reg = MMA8652_CTRL_REG2_RST;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG2, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mma8652_set_active(mma8652_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg |= MMA8652_CTRL_REG1_ACTIVE;

    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mma8652_set_standby(mma8652_t *dev)
{
    char reg;

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg &= ~MMA8652_CTRL_REG1_ACTIVE;

    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return 0;
}

int mma8652_is_ready(mma8652_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MMA8652_STATUS, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    return reg & MMA8652_STATUS_ZYXDR;
}

int mma8652_read(mma8652_t *dev, int16_t *x, int16_t *y, int16_t *z, uint8_t *status)
{
    char buf[7];

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MMA8652_STATUS, buf, 7) != 7) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    *status = buf[0];
    *x = (int16_t)(((int16_t)buf[1] << 8) | buf[2]) / 16;
    *y = (int16_t)(((int16_t)buf[3] << 8) | buf[4]) / 16;
    *z = (int16_t)(((int16_t)buf[5] << 8) | buf[6]) / 16;

    return 0;
}
