/*
 * SPDX-FileCopyrightText: 2026 Arseny Sukhman
 * SPDX-License-Identifier: LGPL-2.1-only
 */


/**
 * @ingroup     drivers_lis331dlh
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LIS331DLH accelerometer
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>

#include "lis331dlh.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C             (dev->params.i2c)
#define DEV_ADDR            (dev->params.addr)

#define REG_WHO_AM_I        (0x0f)
#define REG_CTRL1           (0x20)
#define REG_CTRL2           (0x21)
#define REG_CTRL3           (0x22)
#define REG_CTRL4           (0x23)
#define REG_CTRL5           (0x24)
#define REG_OUT_X_L         (0x28)

#define I2C_AUTO_INC        (0x80)

#define CTRL1_NORMAL_MODE   (0x20)
#define CTRL1_XYZ_ENABLE    (0x07)
#define CTRL1_ODR_SHIFT     (3)

#define CTRL4_BDU           (0x80)
#define CTRL4_FS_2G         (0x00)
#define CTRL4_FS_4G         (0x10)
#define CTRL4_FS_8G         (0x30)

static int _write_reg(const lis331dlh_t *dev, uint8_t reg, uint8_t val)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_write_reg(DEV_I2C, DEV_ADDR, reg, val, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -EIO : 0;
}

static int _read_reg(const lis331dlh_t *dev, uint8_t reg, uint8_t *val)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_reg(DEV_I2C, DEV_ADDR, reg, val, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -EIO : 0;
}

static int _read_regs(const lis331dlh_t *dev, uint8_t reg, void *buf, size_t len)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_ADDR, reg | I2C_AUTO_INC, buf, len, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -EIO : 0;
}

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params)
{
    uint8_t reg;
    int res;

    assert(dev && params);

    dev->params = *params;

    res = _read_reg(dev, REG_WHO_AM_I, &reg);
    if (res < 0) {
        return res;
    }
    if (reg != LIS331DLH_WHO_AM_I) {
        DEBUG("[lis331dlh] unexpected WHO_AM_I: 0x%02x\n", reg);
        return -ENODEV;
    }

    res = lis331dlh_set_scale(dev, params->scale);
    if (res < 0) {
        return res;
    }

    res = lis331dlh_set_odr(dev, params->odr);
    if (res < 0) {
        return res;
    }

    if (_write_reg(dev, REG_CTRL2, 0) < 0 ||
        _write_reg(dev, REG_CTRL3, 0) < 0 ||
        _write_reg(dev, REG_CTRL5, 0) < 0) {
        return -EIO;
    }

    return 0;
}

int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data)
{
    uint8_t buf[6];
    int16_t raw[3];

    assert(dev && data);

    int res = _read_regs(dev, REG_OUT_X_L, buf, sizeof(buf));
    if (res < 0) {
        return res;
    }

    for (unsigned i = 0; i < 3; i++) {
        raw[i] = (int16_t)((uint16_t)buf[(i * 2)] |
                           ((uint16_t)buf[(i * 2) + 1] << 8));
        raw[i] >>= 4;
        raw[i] *= dev->sensitivity;
    }

    data->x = raw[0];
    data->y = raw[1];
    data->z = raw[2];

    return 0;
}

int lis331dlh_set_scale(lis331dlh_t *dev, lis331dlh_scale_t scale)
{
    uint8_t reg;

    assert(dev);

    switch (scale) {
        case LIS331DLH_SCALE_2G:
            reg = CTRL4_BDU | CTRL4_FS_2G;
            dev->sensitivity = 1;
            break;
        case LIS331DLH_SCALE_4G:
            reg = CTRL4_BDU | CTRL4_FS_4G;
            dev->sensitivity = 2;
            break;
        case LIS331DLH_SCALE_8G:
            reg = CTRL4_BDU | CTRL4_FS_8G;
            dev->sensitivity = 4;
            break;
        default:
            return -EINVAL;
    }

    dev->params.scale = scale;
    return _write_reg(dev, REG_CTRL4, reg);
}

int lis331dlh_set_odr(const lis331dlh_t *dev, lis331dlh_odr_t odr)
{
    uint8_t reg;

    assert(dev);

    if (odr > LIS331DLH_ODR_1000HZ) {
        return -EINVAL;
    }

    reg = CTRL1_NORMAL_MODE | ((uint8_t)odr << CTRL1_ODR_SHIFT) |
          CTRL1_XYZ_ENABLE;

    return _write_reg(dev, REG_CTRL1, reg);
}
