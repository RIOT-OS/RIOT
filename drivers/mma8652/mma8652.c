/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2016 OTA keys S.A.
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
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <periph/gpio.h>
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

    if (reg != mma8x5x_device_id[dev->type]) {
        return -1;
    }

    return 0;
}

int mma8652_init(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr, uint8_t range, uint8_t type)
{
    char reg;

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    if (dr > MMA8652_DATARATE_1HZ56 || range > MMA8652_FS_RANGE_8G || type >= MMA8x5x_TYPE_MAX) {
        return -1;
    }

    dev->type = type;

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

    if (mma8652_set_standby(dev) < 0) {
        return -4;
    }

    reg = MMA8652_XYZ_DATA_CFG_FS(range);

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_XYZ_DATA_CFG, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -5;
    }

    reg = MMA8652_CTRL_REG1_DR(dr);

    if (i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -5;
    }
    i2c_release(dev->i2c);

    dev->initialized = true;
    dev->scale = 1024 >> range;

    return 0;
}

#ifdef MODULE_MMA8652_INT

enum mma8652_int_id {
    MMA8652_INT1 = 1,
    MMA8652_INT2 = 2,
};

static const uint8_t MMA8x5x_VALID_INT_SRC[MMA8x5x_TYPE_MAX] = {
    // for MMA8x5x_TYPE_MMA8652
        (MMA8652_INT_SOURCE_DRDY | MMA8652_INT_SOURCE_FF_MT |
        MMA8652_INT_SOURCE_PULSE | MMA8652_INT_SOURCE_LNDPRT |
        MMA8652_INT_SOURCE_TRANS | MMA8652_INT_SOURCE_FIFO |
        MMA8652_INT_SOURCE_ASLP),
    // for MMA8x5x_TYPE_MMA8653
        (MMA8652_INT_SOURCE_DRDY | MMA8652_INT_SOURCE_FF_MT |
        MMA8652_INT_SOURCE_LNDPRT | MMA8652_INT_SOURCE_ASLP),
    // for MMA8x5x_TYPE_MMA8451
        (MMA8652_INT_SOURCE_DRDY | MMA8652_INT_SOURCE_FF_MT |
        MMA8652_INT_SOURCE_PULSE | MMA8652_INT_SOURCE_LNDPRT |
        MMA8652_INT_SOURCE_TRANS | MMA8652_INT_SOURCE_FIFO |
        MMA8652_INT_SOURCE_ASLP),
    // for MMA8x5x_TYPE_MMA8452
        (MMA8652_INT_SOURCE_DRDY | MMA8652_INT_SOURCE_FF_MT |
        MMA8652_INT_SOURCE_PULSE | MMA8652_INT_SOURCE_LNDPRT |
        MMA8652_INT_SOURCE_TRANS | MMA8652_INT_SOURCE_ASLP),
    // for MMA8x5x_TYPE_MMA8453
        (MMA8652_INT_SOURCE_DRDY | MMA8652_INT_SOURCE_FF_MT |
        MMA8652_INT_SOURCE_PULSE | MMA8652_INT_SOURCE_LNDPRT |
        MMA8652_INT_SOURCE_TRANS | MMA8652_INT_SOURCE_ASLP)
};

/**
 * @brief   Get the right interrupt structure for the given mma8x5x device
 *          corresponding to the @p id.
 */
static inline mma8652_int_t *_get_int(mma8652_t *dev, enum mma8652_int_id id)
{
    if (id == MMA8652_INT1) {
        return &(dev->int1);
    }

    else {
        return &(dev->int2);
    }
}

static int mma8652_set_int(mma8652_t *dev, uint8_t int_source, enum mma8652_int_id id);
static int mma8652_add_int(mma8652_t *dev, mma8652_int_t *intx, enum mma8652_int_id id);

int mma8652_init_int(mma8652_t *dev, i2c_t i2c, uint8_t address, uint8_t dr, uint8_t range, uint8_t type,
                     mma8652_int_t *int1, mma8652_int_t *int2)
{
    int res = -1;
    int ret1 = 0;
    int ret2 = 0;

    res = mma8652_init(dev, i2c, address, dr, range, type);
    if (res != 0) {
        return res;
    }

    /* mma8652 interrupt structure should be at least initialized to 0 */
    if (int1 == NULL || int2 == NULL) {
        return -1;
    }

    /* At least, one of interruption pin should be in use */
    if (int1->pin == 0 && int2->pin == 0) {
        return -1;
    }

    if(mma8652_add_int(dev, int1, MMA8652_INT1) == -1) {
        ret1 = -6;
    }

    if(mma8652_add_int(dev, int2, MMA8652_INT2) == -1) {
        ret2 = -7;
    }

    return (ret1 + ret2);
}

static int mma8652_add_int(mma8652_t *dev, mma8652_int_t *intx, uint8_t id)
{
    int res = 0;

    /* pin used */
    if (intx->pin != 0) {
        mma8652_int_t *dev_int = _get_int(dev, id);

        dev_int->pin = intx->pin;

        /* used as an interruption */
        if (intx->cb != NULL) {
            dev_int->cb = intx->cb;
            dev_int->arg = intx->arg;
            gpio_init_int(dev_int->pin, GPIO_IN, GPIO_FALLING, dev_int->cb, dev_int->arg);

            res = mma8652_set_int(dev, intx->source, id);
            if (res != 0) {
                res = -1;
            }
        }
        else {
            gpio_init(dev_int->pin, GPIO_IN);
        }
    }
    return res;
}

static int mma8652_config_int(mma8652_t *dev)
{
    int res = -1;
    uint8_t config = 0;
    uint8_t enable = 0;

    /* INT2 is config dominant on INT1 */
    config = dev->int1.source & ~dev->int2.source;
    enable = dev->int1.source | dev->int2.source;

    DEBUG("Configure mma8652 interruptions\n");

    DEBUG("Device initialized ");
    if (dev->initialized != true) {
        DEBUG("FAILED\n");
        return -EINVAL;
    }
    DEBUG("SUCCESS\n");

    DEBUG("Standby mode ");
    res = mma8652_set_standby(dev);
    if (res != 0) {
        DEBUG("FAILED\n");
        return -EBUSY;
    }
    DEBUG("SUCCESS\n");

    DEBUG("Config interruption ");
    i2c_acquire(dev->i2c);
    res = i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG5, (char *) &config, 1);
    i2c_release(dev->i2c);
    if (res != 1) {
        DEBUG("FAILED\n");
        return -EBUSY;
    }
    DEBUG("SUCCESS\n");

    DEBUG("Enable interruption ");
    i2c_acquire(dev->i2c);
    res = i2c_write_regs(dev->i2c, dev->addr, MMA8652_CTRL_REG4, (char *) &enable, 1);
    i2c_release(dev->i2c);
    if (res != 1) {
        DEBUG("FAILED\n");
        return -EBUSY;
    }
    DEBUG("SUCCESS\n");

    DEBUG("Active mode ");
    res = mma8652_set_active(dev);
    if (res != 0) {
        DEBUG("FAILED\n");
        return -EBUSY;
    }
    DEBUG("SUCCESS\n");

    return 0;
}

static int mma8652_set_int(mma8652_t *dev, uint8_t int_source, uint8_t id)
{
    int res = -1;

    mma8652_int_t *dev_int = _get_int(dev, id);

    if (dev_int->pin == 0 || dev_int->cb == NULL) {
        return -EINVAL;
    }

    /* Force to valid value */
    dev_int->source = int_source & MMA8x5x_VALID_INT_SRC[dev->type];
    res = mma8652_config_int(dev);
    if (res != 0) {
        return res;
    }

    if (dev_int->source != 0) {
        gpio_irq_enable(dev_int->pin);
    }
    else {
        gpio_irq_disable(dev_int->pin);
    }

    return 0;
}

int mma8652_set_int1(mma8652_t *dev, uint8_t int_source)
{
    return mma8652_set_int(dev, int_source, MMA8652_INT1);
}

int mma8652_set_int2(mma8652_t *dev, uint8_t int_source)
{
    return mma8652_set_int(dev, int_source, MMA8652_INT2);
}
#endif

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
    *x = (int32_t)((int16_t)(((int16_t)buf[1] << 8) | buf[2]) >> 4) * 1000 / dev->scale;
    *y = (int32_t)((int16_t)(((int16_t)buf[3] << 8) | buf[4]) >> 4) * 1000 / dev->scale;
    *z = (int32_t)((int16_t)(((int16_t)buf[5] << 8) | buf[6]) >> 4) * 1000 / dev->scale;

    return 0;
}

int mma8682_read_reg(mma8652_t *dev, uint8_t reg, uint8_t *data)
{
    int res = -1;

    i2c_acquire(dev->i2c);
    res = i2c_read_regs(dev->i2c, dev->addr, reg, (char *) data, 1);
    i2c_release(dev->i2c);

    if (res != 1) {
        return -1;
    }

    return 0;
}

int mma8682_read_interruption_status(mma8652_t *dev, uint8_t *data)
{
    return mma8682_read_reg(dev, MMA8652_INT_SOURCE, data);
}

int mma8682_read_pl_status(mma8652_t *dev, uint8_t *data)
{
    return mma8682_read_reg(dev, MMA8652_PL_STATUS, data);
}

int mma8682_read_freefall_motion_status(mma8652_t *dev, uint8_t *data)
{
    return mma8682_read_reg(dev, MMA8652_FF_MT_SRC, data);
}
