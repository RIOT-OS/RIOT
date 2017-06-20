/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @file
 * @brief       Device driver implementation for the LSM6DSL 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include "xtimer.h"

#include "lsm6dsl.h"
#include "lsm6dsl_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int lsm6dsl_init(lsm6dsl_t *dev, const lsm6dsl_params_t *params)
{
    uint8_t tmp;
    int res;

    assert(dev && params);

    dev->params = *params;

    i2c_acquire(dev->params.i2c);
    i2c_init_master(dev->params.i2c, I2C_SPEED_NORMAL);

    /* Reboot */
    i2c_write_reg(dev->params.i2c, dev->params.addr,
                  LSM6DSL_REG_CTRL3_C, LSM6DSL_CTRL3_C_BOOT);

    xtimer_usleep(5000);

    res = i2c_read_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_WHO_AM_I, &tmp);
    if ((res != 1) || (tmp != LSM6DSL_WHO_AM_I)) {
        i2c_release(dev->params.i2c);
        DEBUG("[!!failed!!] WHO_AM_I\n");
        return -1;
    }

    /* Set acc odr / full scale */
    res = i2c_write_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_CTRL1_XL,
                        ((dev->params.acc_odr << LSM6DSL_CTRL_ODR_SHIFT) |
                         (dev->params.acc_fs << LSM6DSL_CTRL_FS_SHIFT)));
    /* Set gyro odr / full scale */
    res += i2c_write_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_CTRL2_G,
                         ((dev->params.gyro_odr << LSM6DSL_CTRL_ODR_SHIFT) |
                          (dev->params.gyro_fs << LSM6DSL_CTRL_FS_SHIFT)));

    /* Set continuous mode */
    uint8_t fifo_odr = MAX(dev->params.acc_odr, dev->params.gyro_odr);
    res += i2c_write_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_FIFO_CTRL5,
                         (fifo_odr << LSM6DSL_FIFO_CTRL5_FIFO_ODR_SHIFT) |
                         LSM6DSL_FIFO_CTRL5_CONTINUOUS_MODE);
    res += i2c_write_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_FIFO_CTRL3,
                         (dev->params.gyro_decimation << LSM6DSL_FIFO_CTRL3_GYRO_DEC_SHIFT) |
                         dev->params.acc_decimation);

    i2c_release(dev->params.i2c);

    if (res < 4) {
        DEBUG("[!!failed!!] config\n");
        return -1;
    }
    return 0;
}

int lsm6dsl_read_acc(lsm6dsl_t *dev, lsm6dsl_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(dev->params.i2c);
    i2c_read_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_STATUS_REG, &tmp);
    DEBUG("lsm6dsl status: %x\n", tmp);

    res = i2c_read_reg(dev->params.i2c, dev->params.addr,
                       LSM6DSL_REG_OUTX_L_XL, &tmp);
    data->x = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTX_H_XL, &tmp);
    data->x |= tmp << 8;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTY_L_XL, &tmp);
    data->y = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTY_H_XL, &tmp);
    data->y |= tmp << 8;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTZ_L_XL, &tmp);
    data->z = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTZ_H_XL, &tmp);
    data->z |= tmp << 8;
    i2c_release(dev->params.i2c);

    if (res < 6) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    float range;
    switch (dev->params.acc_fs) {
    case LSM6DSL_ACC_FS_2G:
        range = 2000.0;
        break;
    case LSM6DSL_ACC_FS_4G:
        range = 4000.0;
        break;
    case LSM6DSL_ACC_FS_8G:
        range = 8000.0;
        break;
    case LSM6DSL_ACC_FS_16G:
        range = 16000.0;
        break;
    default:
        return -1;
    }

    data->x = (data->x * range) / INT16_MAX;
    data->y = (data->y * range) / INT16_MAX;
    data->z = (data->z * range) / INT16_MAX;

    return 0;
}

int lsm6dsl_read_gyro(lsm6dsl_t *dev, lsm6dsl_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(dev->params.i2c);
    i2c_read_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_STATUS_REG, &tmp);
    DEBUG("lsm6dsl status: %x\n", tmp);

    res = i2c_read_reg(dev->params.i2c, dev->params.addr,
                       LSM6DSL_REG_OUTX_L_G, &tmp);
    data->x = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTX_H_G, &tmp);
    data->x |= tmp << 8;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTY_L_G, &tmp);
    data->y = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTY_H_G, &tmp);
    data->y |= tmp << 8;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTZ_L_G, &tmp);
    data->z = tmp;
    res += i2c_read_reg(dev->params.i2c, dev->params.addr,
                        LSM6DSL_REG_OUTZ_H_G, &tmp);
    data->z |= tmp << 8;
    i2c_release(dev->params.i2c);

    if (res < 6) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    float range;
    switch (dev->params.acc_fs) {
    case LSM6DSL_GYRO_FS_245DPS:
        range = 2450.0;
        break;
    case LSM6DSL_GYRO_FS_500DPS:
        range = 5000.0;
        break;
    case LSM6DSL_GYRO_FS_1000DPS:
        range = 10000.0;
        break;
    case LSM6DSL_GYRO_FS_2000DPS:
        range = 20000.0;
        break;
    default:
        return -1;
    }

    data->x = (data->x * range) / INT16_MAX;
    data->y = (data->y * range) / INT16_MAX;
    data->z = (data->z * range) / INT16_MAX;

    return 0;
}

int lsm6dsl_read_temp(lsm6dsl_t *dev, int16_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(dev->params.i2c);
    res = i2c_read_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_OUT_TEMP_L, &tmp);
    *data = tmp;

    res += i2c_read_reg(dev->params.i2c, dev->params.addr, LSM6DSL_REG_OUT_TEMP_H, &tmp);
    *data |= tmp << 8;

    i2c_release(dev->params.i2c);

    if (res < 2) {
        return -1;
    }

    return 0;
}
