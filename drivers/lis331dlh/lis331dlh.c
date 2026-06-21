/*
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "lis331dlh.h"

#define LIS331DLH_REG_WHO_AM_I   (0x0f)
#define LIS331DLH_REG_CTRL_REG1  (0x20)
#define LIS331DLH_REG_CTRL_REG4  (0x23)
#define LIS331DLH_REG_OUT_X_L    (0x28)

#define LIS331DLH_WHO_AM_I       (0x32)

#define LIS331DLH_CTRL_REG1_NORMAL_MODE  (0x20)
#define LIS331DLH_CTRL_REG1_XYZ_ENABLE   (0x07)
#define LIS331DLH_CTRL_REG4_BDU          (0x80)

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
        return -1;
    }

    if (_read_reg(dev, reg + 1, &high) != 0) {
        return -1;
    }

    *out = (int16_t)((high << 8) | low);
    return 0;
}

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params)
{
    uint8_t who_am_i = 0;

    dev->params = params;

    i2c_init(dev->params->i2c);

    if (_read_reg(dev, LIS331DLH_REG_WHO_AM_I, &who_am_i) != 0) {
        return -1;
    }

    if (who_am_i != LIS331DLH_WHO_AM_I) {
        return -2;
    }

    uint8_t ctrl1 = LIS331DLH_CTRL_REG1_NORMAL_MODE |
                    dev->params->rate |
                    LIS331DLH_CTRL_REG1_XYZ_ENABLE;

    if (_write_reg(dev, LIS331DLH_REG_CTRL_REG1, ctrl1) != 0) {
        return -1;
    }

    uint8_t ctrl4 = LIS331DLH_CTRL_REG4_BDU | dev->params->scale;

    if (_write_reg(dev, LIS331DLH_REG_CTRL_REG4, ctrl4) != 0) {
        return -1;
    }

    return 0;
}

int lis331dlh_read_xyz(const lis331dlh_t *dev, lis331dlh_data_t *data)
{
    if (_read_axis(dev, LIS331DLH_REG_OUT_X_L, &data->x) != 0) {
        return -1;
    }

    if (_read_axis(dev, LIS331DLH_REG_OUT_X_L + 2, &data->y) != 0) {
        return -1;
    }

    return _read_axis(dev, LIS331DLH_REG_OUT_X_L + 4, &data->z);
}
