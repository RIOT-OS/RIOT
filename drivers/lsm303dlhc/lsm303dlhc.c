/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lsm303dlhc
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM303DLHC 3D accelerometer/magnetometer.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "lsm303dlhc.h"
#include "lsm303dlhc-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DEV_I2C         (dev->params.i2c)
#define DEV_ACC_ADDR    (dev->params.acc_addr)
#define DEV_ACC_PIN     (dev->params.acc_pin)
#define DEV_ACC_RATE    (dev->params.acc_rate)
#define DEV_ACC_SCALE   (dev->params.acc_scale)
#define DEV_MAG_ADDR    (dev->params.mag_addr)
#define DEV_MAG_PIN     (dev->params.mag_pin)
#define DEV_MAG_RATE    (dev->params.mag_rate)
#define DEV_MAG_GAIN    (dev->params.mag_gain)

int lsm303dlhc_init(lsm303dlhc_t *dev, const lsm303dlhc_params_t *params)
{
    dev->params = *params;

    int res;
    uint8_t tmp;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);

    DEBUG("lsm303dlhc reboot...");
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_CTRL5_A, LSM303DLHC_REG_CTRL5_A_BOOT, 0);
    /* Release the bus for other threads. */
    i2c_release(DEV_I2C);
    DEBUG("[OK]\n");

    /* configure accelerometer */
    /* enable all three axis and set sample rate */
    tmp = (LSM303DLHC_CTRL1_A_XEN
          | LSM303DLHC_CTRL1_A_YEN
          | LSM303DLHC_CTRL1_A_ZEN
          | DEV_ACC_RATE);
    i2c_acquire(DEV_I2C);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303DLHC_REG_CTRL1_A, tmp, 0);
    /* update on read, MSB @ low address, scale and high-resolution */
    tmp = (DEV_ACC_SCALE | LSM303DLHC_CTRL4_A_HR);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303DLHC_REG_CTRL4_A, tmp, 0);
    /* no interrupt generation */
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303DLHC_REG_CTRL3_A, LSM303DLHC_CTRL3_A_I1_NONE, 0);
    /* configure acc data ready pin */
    gpio_init(DEV_ACC_PIN, GPIO_IN);

    /* configure magnetometer and temperature */
    /* enable temperature output and set sample rate */
    tmp = LSM303DLHC_TEMP_EN | DEV_MAG_RATE;
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303DLHC_REG_CRA_M, tmp, 0);
    /* configure z-axis gain */
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303DLHC_REG_CRB_M, DEV_MAG_GAIN, 0);
    /* set continuous mode */
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_CONTINUOUS, 0);
    i2c_release(DEV_I2C);
    /* configure mag data ready pin */
    gpio_init(DEV_MAG_PIN, GPIO_IN);

    return (res < 0) ? -1 : 0;
}

int lsm303dlhc_read_acc(const lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    i2c_read_reg(DEV_I2C, DEV_ACC_ADDR, LSM303DLHC_REG_STATUS_A, &tmp, 0);
    DEBUG("lsm303dlhc status: %x\n", tmp);
    DEBUG("lsm303dlhc: wait for acc values ... ");

    res = i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303DLHC_REG_OUT_X_L_A, &tmp, 0);
    data->x_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_OUT_X_H_A, &tmp, 0);
    data->x_axis |= tmp<<8;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303DLHC_REG_OUT_Y_L_A, &tmp, 0);
    data->y_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_OUT_Y_H_A, &tmp, 0);
    data->y_axis |= tmp<<8;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303DLHC_REG_OUT_Z_L_A, &tmp, 0);
    data->z_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_OUT_Z_H_A, &tmp, 0);
    data->z_axis |= tmp<<8;
    i2c_release(DEV_I2C);
    DEBUG("read ... ");

    data->x_axis = data->x_axis>>4;
    data->y_axis = data->y_axis>>4;
    data->z_axis = data->z_axis>>4;

    if (res < 0) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    return 0;
}

int lsm303dlhc_read_mag(const lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data)
{
    int res;

    DEBUG("lsm303dlhc: wait for mag values... ");
    while (gpio_read(DEV_MAG_PIN) == 0){}

    DEBUG("read ... ");

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_MAG_ADDR,
                        LSM303DLHC_REG_OUT_X_H_M, data, 6, 0);
    i2c_release(DEV_I2C);

    if (res < 0) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    /* interchange y and z axis and fix endiness */
    int16_t tmp = data->y_axis;
    data->x_axis = ((data->x_axis<<8)|((data->x_axis>>8)&0xff));
    data->y_axis = ((data->z_axis<<8)|((data->z_axis>>8)&0xff));
    data->z_axis = ((tmp<<8)|((tmp>>8)&0xff));

    /* compensate z-axis sensitivity */
    /* gain is currently hardcoded to LSM303DLHC_GAIN_5 */
    data->z_axis = ((data->z_axis * 400) / 355);

    return 0;
}

int lsm303dlhc_read_temp(const lsm303dlhc_t *dev, int16_t *value)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_MAG_ADDR, LSM303DLHC_REG_TEMP_OUT_H,
                        value, 2, 0);
    i2c_release(DEV_I2C);

    if (res < 0) {
        return -1;
    }

    *value = (((*value) >> 8) & 0xff) | (*value << 8);

    DEBUG("LSM303DLHC: raw temp: %i\n", *value);

    return 0;
}

int lsm303dlhc_disable(const lsm303dlhc_t *dev)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_CTRL1_A, LSM303DLHC_CTRL1_A_POWEROFF, 0);
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_SLEEP, 0);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303DLHC_REG_CRA_M, LSM303DLHC_TEMP_DIS, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -1 : 0;
}

int lsm303dlhc_enable(const lsm303dlhc_t *dev)
{
    int res;
    uint8_t tmp = (LSM303DLHC_CTRL1_A_XEN
                  | LSM303DLHC_CTRL1_A_YEN
                  | LSM303DLHC_CTRL1_A_ZEN
                  | LSM303DLHC_CTRL1_A_N1344HZ_L5376HZ);
    i2c_acquire(DEV_I2C);
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303DLHC_REG_CTRL1_A, tmp, 0);

    tmp = (LSM303DLHC_CTRL4_A_BDU| LSM303DLHC_CTRL4_A_SCALE_2G | LSM303DLHC_CTRL4_A_HR);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303DLHC_REG_CTRL4_A, tmp, 0);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303DLHC_REG_CTRL3_A,
                         LSM303DLHC_CTRL3_A_I1_DRDY1, 0);
    gpio_init(DEV_ACC_PIN, GPIO_IN);

    tmp = LSM303DLHC_TEMP_EN | LSM303DLHC_TEMP_SAMPLE_75HZ;
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR, LSM303DLHC_REG_CRA_M, tmp, 0);

    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303DLHC_REG_CRB_M, LSM303DLHC_GAIN_5, 0);

    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_CONTINUOUS, 0);
    i2c_release(DEV_I2C);

    gpio_init(DEV_MAG_PIN, GPIO_IN);

    return (res < 0) ? -1 : 0;
}
