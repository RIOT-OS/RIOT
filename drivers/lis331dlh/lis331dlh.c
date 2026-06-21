/*
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "lis331dlh.h"

#define LIS331DLH_REG_WHO_AM_I   (0x0f)
#define LIS331DLH_REG_CTRL_REG1  (0x20)
#define LIS331DLH_REG_CTRL_REG4  (0x23)
#define LIS331DLH_REG_OUT_X_L    (0x28)

#define LIS331DLH_WHO_AM_I       (0x32)

static int _write_reg(const lis331dlh_t *dev, uint8_t reg, uint8_t val)
{
    i2c_acquire(dev->params->i2c);
    int res = i2c_write_reg(dev->params->i2c, dev->params->addr, reg, val, 0);
    i2c_release(dev->params->i2c);

    return res;
}

static int _read_reg(const lis331dlh_t *dev, uint8_t reg, uint8_t *val)
{
    i2c_acquire(dev->params->i2c);
    int res = i2c_read_reg(dev->params->i2c, dev->params->addr, reg, val, 0);
    i2c_release(dev->params->i2c);

    return res;
}

static int _read_axis(const lis331dlh_t *dev, uint8_t reg, int16_t *out)
{
    uint8_t low = 0;
    uint8_t high = 0;

    if (_read_reg(dev, reg, &low) != 0) {
        return LIS331DLH_ERROR;
    }

    if (_read_reg(dev, reg + 1, &high) != 0) {
        return LIS331DLH_ERROR;
    }

    *out = (int16_t)((high << 8) | low);
    return LIS331DLH_OK;
}

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params)
{
    uint8_t who_am_i = 0;

    dev->params = params;

    i2c_init(dev->params->i2c);

    if (_read_reg(dev, LIS331DLH_REG_WHO_AM_I, &who_am_i) != 0) {
        return LIS331DLH_ERROR;
    }

    if (who_am_i != LIS331DLH_WHO_AM_I) {
        return LIS331DLH_NODEV;
    }

    if (_write_reg(dev, LIS331DLH_REG_CTRL_REG1, 0x27) != 0) {
        return LIS331DLH_ERROR;
    }

    if (_write_reg(dev, LIS331DLH_REG_CTRL_REG4, 0x00) != 0) {
        return LIS331DLH_ERROR;
    }

    return LIS331DLH_OK;
}

int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data)
{
    if (_read_axis(dev, LIS331DLH_REG_OUT_X_L, &data->x) != LIS331DLH_OK) {
        return LIS331DLH_ERROR;
    }

    if (_read_axis(dev, LIS331DLH_REG_OUT_X_L + 2, &data->y) != LIS331DLH_OK) {
        return LIS331DLH_ERROR;
    }

    return _read_axis(dev, LIS331DLH_REG_OUT_X_L + 4, &data->z);
}
