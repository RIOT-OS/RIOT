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
    res = i2c_read_regs(DEV_I2C, DEV_ADDR,
                        reg | LIS331DLH_I2C_AUTO_INC, buf, len, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -EIO : 0;
}

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params)
{
    uint8_t reg;
    int res;

    assert(dev && params);

    dev->params = *params;

    res = _read_reg(dev, LIS331DLH_REG_WHO_AM_I, &reg);
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

    if (_write_reg(dev, LIS331DLH_REG_CTRL2, 0) < 0 ||
        _write_reg(dev, LIS331DLH_REG_CTRL3, 0) < 0 ||
        _write_reg(dev, LIS331DLH_REG_CTRL5, 0) < 0) {
        return -EIO;
    }

    return 0;
}

int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data)
{
    uint8_t buf[6];
    int16_t raw[3];

    assert(dev && data);

    int res = _read_regs(dev, LIS331DLH_REG_OUT_X_L, buf, sizeof(buf));
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
    int16_t sensitivity;
    int res;

    assert(dev);

    switch (scale) {
    case LIS331DLH_SCALE_2G:
        reg = LIS331DLH_CTRL4_BDU | LIS331DLH_CTRL4_FS_2G;
        sensitivity = 1;
        break;
    case LIS331DLH_SCALE_4G:
        reg = LIS331DLH_CTRL4_BDU | LIS331DLH_CTRL4_FS_4G;
        sensitivity = 2;
        break;
    case LIS331DLH_SCALE_8G:
        reg = LIS331DLH_CTRL4_BDU | LIS331DLH_CTRL4_FS_8G;
        sensitivity = 4;
        break;
    default:
        return -EINVAL;
    }

    res = _write_reg(dev, LIS331DLH_REG_CTRL4, reg);
    if (res < 0) {
        return res;
    }

    dev->sensitivity = sensitivity;
    dev->params.scale = scale;
    return 0;
}

int lis331dlh_set_odr(lis331dlh_t *dev, lis331dlh_odr_t odr)
{
    uint8_t reg;
    int res;

    assert(dev);

    if (odr > LIS331DLH_ODR_1000HZ) {
        return -EINVAL;
    }

    reg = LIS331DLH_CTRL1_NORMAL_MODE |
          ((uint8_t)odr << LIS331DLH_CTRL1_ODR_SHIFT) |
          LIS331DLH_CTRL1_XYZ_ENABLE;

    res = _write_reg(dev, LIS331DLH_REG_CTRL1, reg);
    if (res < 0) {
        return res;
    }

    dev->params.odr = odr;
    return 0;
}
