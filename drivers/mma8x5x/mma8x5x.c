/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mma8x5x
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MMA8x5x accelerometer family
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "periph/i2c.h"
#include "mma8x5x.h"
#include "mma8x5x_regs.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define I2C_SPEED           I2C_SPEED_FAST

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)

int mma8x5x_init(mma8x5x_t *dev, const mma8x5x_params_t *params)
{
    uint8_t reg;

    assert(dev && params);

    /* write device descriptor */
    memcpy(dev, params, sizeof(mma8x5x_params_t));

    /* initialize the I2C bus */
    i2c_acquire(BUS);
    if (i2c_init_master(BUS, I2C_SPEED) < 0) {
        i2c_release(BUS);
        DEBUG("[mma8x5x] init - error: unable to initialize I2C bus\n");
        return MMA8X5X_NOI2C;
    }

    /* test if the target device responds */
    i2c_read_reg(BUS, ADDR, MMA8X5X_WHO_AM_I, &reg);
    if (reg != dev->params.type) {
        i2c_release(BUS);
        DEBUG("[mma8x5x] init - error: invalid WHO_AM_I value [0x%02x]\n",
               (int)reg);
        return MMA8X5X_NODEV;
    }

    /* reset the device */
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG2, MMA8X5X_CTRL_REG2_RST);
    do {
        i2c_read_reg(BUS, ADDR, MMA8X5X_CTRL_REG2, &reg);
    } while (reg & MMA8X5X_CTRL_REG2_RST);
    /* configure the user offset */
    i2c_write_regs(BUS, ADDR, MMA8X5X_OFF_X, dev->params.offset, 3);
    /* configure range, rate, and activate the device */
    reg = (dev->params.range & MMA8X5X_XYZ_DATA_CFG_FS_MASK);
    i2c_write_reg(BUS, ADDR, MMA8X5X_XYZ_DATA_CFG, reg);
    reg = ((dev->params.rate & MMA8X5X_CTRL_REG1_DR_MASK) |
           MMA8X5X_CTRL_REG1_ACTIVE);
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, reg);

    /* finally release the bus */
    i2c_release(BUS);

    DEBUG("[mma8x5x] init: successful\n");

    return MMA8X5X_OK;
}

void mma8x5x_set_user_offset(mma8x5x_t *dev, int8_t x, int8_t y, int8_t z)
{
    uint8_t buf[3];

    assert(dev);

    buf[0] = (uint8_t)x;
    buf[1] = (uint8_t)y;
    buf[2] = (uint8_t)z;

    DEBUG("[mma8x5x] setting user offset to X: %3i, Y: %3i, Z: %3i\n",
          (int)x, (int)y, (int)z);

    i2c_acquire(BUS);
    i2c_write_regs(BUS, ADDR, MMA8X5X_OFF_X, buf, 3);
    i2c_release(BUS);
}

void mma8x5x_set_active(mma8x5x_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[mma8x5x] put device to active mode\n");

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, &reg);
    reg |= MMA8X5X_CTRL_REG1_ACTIVE;
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, reg);
    i2c_release(BUS);
}

void mma8x5x_set_standby(mma8x5x_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[mma8x5x] put device to standby mode\n");

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, &reg);
    reg &= ~MMA8X5X_CTRL_REG1_ACTIVE;
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, reg);
    i2c_release(BUS);
}

int mma8x5x_is_ready(mma8x5x_t *dev)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[mma8x5x] checking for new available data\n");

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, MMA8X5X_STATUS, &reg);
    i2c_release(BUS);

    if (reg & MMA8X5X_STATUS_ZYXDR) {
        return MMA8X5X_DATA_READY;
    }
    else {
        return MMA8X5X_NODATA;
    }
}

void mma8x5x_read(mma8x5x_t *dev, mma8x5x_data_t *data)
{
    uint8_t buf[7];

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, MMA8X5X_STATUS, buf, 7);
    i2c_release(BUS);

    data->x = ((int16_t)(buf[1] << 8 | buf[2])) / (16 >> dev->params.range);
    data->y = ((int16_t)(buf[3] << 8 | buf[4])) / (16 >> dev->params.range);
    data->z = ((int16_t)(buf[5] << 8 | buf[6])) / (16 >> dev->params.range);
}
