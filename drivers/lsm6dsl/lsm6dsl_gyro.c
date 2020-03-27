/*
 * Copyright (C) 2020 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM6DSL gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Sylvain Fabre <sylvain.fabre-ext@continental-its.com>
 *
 * @}
 */

#include "xtimer.h"

#include "lsm6dsl.h"
#include "lsm6dsl_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

/**
 * order in array [0, 1, 2, 3] is
 * LSM6DSL_GYRO_FS_245DPS, LSM6DSL_GYRO_FS_500DPS,
 * LSM6DSL_GYRO_FS_1000DPS, LSM6DSL_GYRO_FS_2000DPS
 */
static const int16_t range_gyro[] = { 2450, 5000, 10000, 20000 };

int lsm6dsl_read_gyro(const lsm6dsl_t *dev, lsm6dsl_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADDR, LSM6DSL_REG_STATUS_REG, &tmp, 0);
    DEBUG("lsm6dsl status: %x\n", tmp);

    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTX_L_G, &tmp, 0);
    data->x = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTX_H_G, &tmp, 0);
    data->x |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTY_L_G, &tmp, 0);
    data->y = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTY_H_G, &tmp, 0);
    data->y |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTZ_L_G, &tmp, 0);
    data->z = tmp;
    res += i2c_read_reg(BUS, ADDR, LSM6DSL_REG_OUTZ_H_G, &tmp, 0);
    data->z |= tmp << 8;
    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_read_gyro\n");
        return -LSM6DSL_ERROR_BUS;
    }

    assert(dev->params.gyro_fs < LSM6DSL_GYRO_FS_MAX);
    data->x = ((int32_t)data->x * range_gyro[dev->params.gyro_fs]) / INT16_MAX;
    data->y = ((int32_t)data->y * range_gyro[dev->params.gyro_fs]) / INT16_MAX;
    data->z = ((int32_t)data->z * range_gyro[dev->params.gyro_fs]) / INT16_MAX;

    return LSM6DSL_OK;
}

int lsm6dsl_gyro_power_down(const lsm6dsl_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_CTRL2_G, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dsl_gyro_power_down\n");
        return -LSM6DSL_ERROR_BUS;
    }

    tmp &= ~(LSM6DSL_CTRL_ODR_MASK);
    res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL2_G, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_gyro_power_down\n");
        return -LSM6DSL_ERROR_BUS;
    }

    return LSM6DSL_OK;
}

int lsm6dsl_gyro_power_up(const lsm6dsl_t *dev)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, LSM6DSL_REG_CTRL2_G, &tmp, 0);
    if (res < 0) {
        i2c_release(BUS);
        DEBUG("[ERROR] lsm6dsl_gyro_power_up\n");
        return -LSM6DSL_ERROR_BUS;
    }

    tmp &= ~(LSM6DSL_CTRL_ODR_MASK);
    tmp |= dev->params.gyro_odr << LSM6DSL_CTRL_ODR_SHIFT;
    res = i2c_write_reg(BUS, ADDR, LSM6DSL_REG_CTRL2_G, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] lsm6dsl_gyro_power_up\n");
        return -LSM6DSL_ERROR_BUS;
    }

    return LSM6DSL_OK;
}
