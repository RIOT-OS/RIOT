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

#define ENABLE_DEBUG        0
#include "debug.h"

#define BUS                 (dev->params.i2c)
#define ADDR                (dev->params.addr)

int mma8x5x_init(mma8x5x_t *dev, const mma8x5x_params_t *params)
{
    uint8_t reg = 0;

    assert(dev && params);

    /* write device descriptor */
    dev->params = *params;

    /* acquire the I2C bus */
    i2c_acquire(BUS);

    /* test if the target device responds */
    i2c_read_reg(BUS, ADDR, MMA8X5X_WHO_AM_I, &reg, 0);
    switch (reg) {
        case MMA8X5X_TYPE_MMA8451:
        case MMA8X5X_TYPE_MMA8452:
        case MMA8X5X_TYPE_MMA8453:
        case MMA8X5X_TYPE_MMA8652:
        case MMA8X5X_TYPE_MMA8653:
            break;
        default: /* invalid device type */
            i2c_release(BUS);
            DEBUG("[mma8x5x] init - error: invalid WHO_AM_I value [0x%02x]\n",
                  (int)reg);
            return MMA8X5X_NODEV;
    }

    /* reset the device */
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG2, MMA8X5X_CTRL_REG2_RST, 0);
    do {
        i2c_read_reg(BUS, ADDR, MMA8X5X_CTRL_REG2, &reg, 0);
    } while (reg & MMA8X5X_CTRL_REG2_RST);
    /* configure the user offset */
    i2c_write_regs(BUS, ADDR, MMA8X5X_OFF_X, dev->params.offset, 3, 0);
    /* configure range, rate, and activate the device */
    reg = (dev->params.range & MMA8X5X_XYZ_DATA_CFG_FS_MASK);
    i2c_write_reg(BUS, ADDR, MMA8X5X_XYZ_DATA_CFG, reg, 0);
    reg = ((dev->params.rate & MMA8X5X_CTRL_REG1_DR_MASK) |
           MMA8X5X_CTRL_REG1_ACTIVE);
    i2c_write_reg(BUS, ADDR, MMA8X5X_CTRL_REG1, reg, 0);

    /* finally release the bus */
    i2c_release(BUS);

    DEBUG("[mma8x5x] init: successful\n");

    return MMA8X5X_OK;
}

void mma8x5x_set_user_offset(const mma8x5x_t *dev, int8_t x, int8_t y, int8_t z)
{
    uint8_t buf[3];

    assert(dev);

    buf[0] = (uint8_t)x;
    buf[1] = (uint8_t)y;
    buf[2] = (uint8_t)z;

    DEBUG("[mma8x5x] setting user offset to X: %3i, Y: %3i, Z: %3i\n",
          (int)x, (int)y, (int)z);

    i2c_acquire(BUS);
    i2c_write_regs(BUS, ADDR, MMA8X5X_OFF_X, buf, 3, 0);
    i2c_release(BUS);
}

static int _get_reg(const mma8x5x_t *dev, uint8_t addr)
{
    uint8_t reg;

    assert(dev);

    DEBUG("[mma8x5x] getting reg 0x%02x\n", (unsigned)addr);

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, addr, &reg, 0);
    i2c_release(BUS);

    DEBUG("[mma8x5x] reg 0x%02x=0x%02x\n", (unsigned)addr, (unsigned)reg);
    return reg;
}

static void _reg_setbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val)
{
    uint8_t tmp;

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, reg, &tmp, 0);
    DEBUG("[mma8x5x] 0x%02x: 0x%02x | 0x%02x = 0x%02x\n",
            (unsigned)reg, (unsigned)tmp, (unsigned)val, (unsigned) tmp | val);
    tmp |= val;
    i2c_write_reg(BUS, ADDR, reg, tmp, 0);
    i2c_release(BUS);
}

static void _reg_clearbits(const mma8x5x_t *dev, uint8_t reg, uint8_t val)
{
    uint8_t tmp;

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, reg, &tmp, 0);
    DEBUG("[mma8x5x] 0x%02x: 0x%02x &= ~0x%02x = 0x%02x\n",
            (unsigned)reg, (unsigned)tmp, (unsigned)val, (unsigned) tmp & ~val);
    tmp &= ~val;
    i2c_write_reg(BUS, ADDR, reg, tmp, 0);
    i2c_release(BUS);
}

void mma8x5x_set_active(const mma8x5x_t *dev)
{
    DEBUG("[mma8x5x] put device to active mode\n");
    _reg_setbits(dev, MMA8X5X_CTRL_REG1, MMA8X5X_CTRL_REG1_ACTIVE);
}

void mma8x5x_set_standby(const mma8x5x_t *dev)
{
    DEBUG("[mma8x5x] put device to standby mode\n");
    _reg_clearbits(dev, MMA8X5X_CTRL_REG1, MMA8X5X_CTRL_REG1_ACTIVE);
}

void mma8x5x_set_motiondetect(const mma8x5x_t *dev, uint8_t int_pin, uint8_t threshold)
{
    DEBUG("[mma8x5x] put device to motion detect mode (ELE=1, OAE=1)\n");
    assert(int_pin < 3);

    mma8x5x_set_standby(dev);

    _reg_setbits(dev, MMA8X5X_FF_MT_CFG, \
            MMA8X5X_FF_MT_CFG_XEFE | \
            MMA8X5X_FF_MT_CFG_YEFE | \
            MMA8X5X_FF_MT_CFG_ZEFE | \
            MMA8X5X_FF_MT_CFG_OAE | MMA8X5X_FF_MT_CFG_ELE);

    switch(int_pin) {
        case 0:
            _reg_clearbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_FF_MT);
            goto out;
        case 1:
            _reg_setbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_FF_MT);
            break;
        case 2:
            _reg_clearbits(dev, MMA8X5X_CTRL_REG5, MMA8X5X_CTRL_REG5_INT_CFG_FF_MT);
            break;
    }

    _reg_setbits(dev, MMA8X5X_FF_MT_THS, threshold & 0x7f);
    _reg_setbits(dev, MMA8X5X_CTRL_REG4, MMA8X5X_CTRL_REG4_INT_EN_FF_MT);

out:
    mma8x5x_set_active(dev);
}

void mma8x5x_ack_int(const mma8x5x_t *dev)
{
    _get_reg(dev, MMA8X5X_FF_MT_SRC);
}

int mma8x5x_is_ready(const mma8x5x_t *dev)
{
    DEBUG("[mma8x5x] checking for new available data\n");

    uint8_t reg = _get_reg(dev, MMA8X5X_STATUS);

    if (reg & MMA8X5X_STATUS_ZYXDR) {
        return MMA8X5X_DATA_READY;
    }
    else {
        return MMA8X5X_NODATA;
    }
}

void mma8x5x_read(const mma8x5x_t *dev, mma8x5x_data_t *data)
{
    uint8_t buf[7];

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, MMA8X5X_STATUS, buf, 7, 0);
    i2c_release(BUS);

    data->x = ((int16_t)(buf[1] << 8 | buf[2])) / (16 >> dev->params.range);
    data->y = ((int16_t)(buf[3] << 8 | buf[4])) / (16 >> dev->params.range);
    data->z = ((int16_t)(buf[5] << 8 | buf[6])) / (16 >> dev->params.range);
}
