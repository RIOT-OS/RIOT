/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lsm303dlhc
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM303DLHC light sensor
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

int lsm303dlhc_init(lsm303dlhc_t *dev, i2c_t i2c, gpio_t acc_pin, gpio_t mag_pin,
                    uint8_t acc_address,
                    lsm303dlhc_acc_sample_rate_t acc_sample_rate,
                    lsm303dlhc_acc_scale_t acc_scale,
                    uint8_t mag_address,
                    lsm303dlhc_mag_sample_rate_t mag_sample_rate,
                    lsm303dlhc_mag_gain_t mag_gain)
{
    int res;
    char tmp;

    dev->i2c = i2c;
    dev->acc_address = acc_address;
    dev->mag_address = mag_address;
    dev->acc_pin     = acc_pin;
    dev->mag_pin     = mag_pin;
    dev->acc_scale   = acc_scale;
    dev->mag_gain    = mag_gain;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    i2c_init_master(i2c, I2C_SPEED_NORMAL);

    DEBUG("lsm303dlhc reboot ");
    res = i2c_write_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_CTRL5_A, LSM303DLHC_REG_CTRL5_A_BOOT);
    /* Release the bus for other threads. */
    i2c_release(dev->i2c);
    DEBUG("[OK]");

    /* configure accelerometer */
    /* enable all three axis and set sample rate */
    tmp = (LSM303DLHC_CTRL1_A_XEN
          | LSM303DLHC_CTRL1_A_YEN
          | LSM303DLHC_CTRL1_A_ZEN
          | acc_sample_rate);
    i2c_acquire(dev->i2c);
    res += i2c_write_reg(dev->i2c, dev->acc_address,
                         LSM303DLHC_REG_CTRL1_A, tmp);
    /* update on read, MSB @ low address, scale and high-resolution */
    tmp = (acc_scale | LSM303DLHC_CTRL4_A_HR);
    res += i2c_write_reg(dev->i2c, dev->acc_address,
                         LSM303DLHC_REG_CTRL4_A, tmp);
    /* no interrupt generation */
    res += i2c_write_reg(dev->i2c, dev->acc_address,
                         LSM303DLHC_REG_CTRL3_A, LSM303DLHC_CTRL3_A_I1_NONE);
    /* configure acc data ready pin */
    gpio_init(acc_pin, GPIO_IN);

    /* configure magnetometer and temperature */
    /* enable temperature output and set sample rate */
    tmp = LSM303DLHC_TEMP_EN | mag_sample_rate;
    res += i2c_write_reg(dev->i2c, dev->mag_address,
                         LSM303DLHC_REG_CRA_M, tmp);
    /* configure z-axis gain */
    res += i2c_write_reg(dev->i2c, dev->mag_address,
                         LSM303DLHC_REG_CRB_M, mag_gain);
    /* set continuous mode */
    res += i2c_write_reg(dev->i2c, dev->mag_address,
                         LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_CONTINUOUS);
    i2c_release(dev->i2c);
    /* configure mag data ready pin */
    gpio_init(mag_pin, GPIO_IN);

    return (res < 7) ? -1 : 0;
}

int lsm303dlhc_read_acc(lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data)
{
    int res;
    char tmp;

    i2c_acquire(dev->i2c);
    i2c_read_reg(dev->i2c, dev->acc_address, LSM303DLHC_REG_STATUS_A, &tmp);
    DEBUG("lsm303dlhc status: %x\n", tmp);
    DEBUG("lsm303dlhc: wait for acc values ... ");

    res = i2c_read_reg(dev->i2c, dev->acc_address,
                       LSM303DLHC_REG_OUT_X_L_A, &tmp);
    data->x_axis = tmp;
    res += i2c_read_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_OUT_X_H_A, &tmp);
    data->x_axis |= tmp<<8;
    res += i2c_read_reg(dev->i2c, dev->acc_address,
                       LSM303DLHC_REG_OUT_Y_L_A, &tmp);
    data->y_axis = tmp;
    res += i2c_read_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_OUT_Y_H_A, &tmp);
    data->y_axis |= tmp<<8;
    res += i2c_read_reg(dev->i2c, dev->acc_address,
                       LSM303DLHC_REG_OUT_Z_L_A, &tmp);
    data->z_axis = tmp;
    res += i2c_read_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_OUT_Z_H_A, &tmp);
    data->z_axis |= tmp<<8;
    i2c_release(dev->i2c);
    DEBUG("read ... ");

    data->x_axis = data->x_axis>>4;
    data->y_axis = data->y_axis>>4;
    data->z_axis = data->z_axis>>4;

    if (res < 6) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    return 0;
}

int lsm303dlhc_read_mag(lsm303dlhc_t *dev, lsm303dlhc_3d_data_t *data)
{
    int res;

    DEBUG("lsm303dlhc: wait for mag values... ");
    while (gpio_read(dev->mag_pin) == 0){}

    DEBUG("read ... ");

    i2c_acquire(dev->i2c);
    res = i2c_read_regs(dev->i2c, dev->mag_address,
                        LSM303DLHC_REG_OUT_X_H_M, (char*)data, 6);
    i2c_release(dev->i2c);

    if (res < 6) {
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

int lsm303dlhc_read_temp(lsm303dlhc_t *dev, int16_t *value)
{
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_read_regs(dev->i2c, dev->mag_address, LSM303DLHC_REG_TEMP_OUT_H, (char*)value, 2);
    i2c_release(dev->i2c);

    if (res < 2) {
        return -1;
    }

    *value = (((*value) >> 8) & 0xff) | (*value << 8);

    DEBUG("LSM303DLHC: raw temp: %i\n", *value);

    return 0;
}

int lsm303dlhc_disable(lsm303dlhc_t *dev)
{
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_write_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_CTRL1_A, LSM303DLHC_CTRL1_A_POWEROFF);
    res += i2c_write_reg(dev->i2c, dev->mag_address,
                        LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_SLEEP);
    res += i2c_write_reg(dev->i2c, dev->acc_address,
                        LSM303DLHC_REG_CRA_M, LSM303DLHC_TEMP_DIS);
    i2c_release(dev->i2c);

    return (res < 3) ? -1 : 0;
}

int lsm303dlhc_enable(lsm303dlhc_t *dev)
{
    int res;
    char tmp = (LSM303DLHC_CTRL1_A_XEN
                | LSM303DLHC_CTRL1_A_YEN
                | LSM303DLHC_CTRL1_A_ZEN
                | LSM303DLHC_CTRL1_A_N1344HZ_L5376HZ);
    i2c_acquire(dev->i2c);
    res = i2c_write_reg(dev->i2c, dev->acc_address, LSM303DLHC_REG_CTRL1_A, tmp);

    tmp = (LSM303DLHC_CTRL4_A_BDU| LSM303DLHC_CTRL4_A_SCALE_2G | LSM303DLHC_CTRL4_A_HR);
    res += i2c_write_reg(dev->i2c, dev->acc_address, LSM303DLHC_REG_CTRL4_A, tmp);
    res += i2c_write_reg(dev->i2c, dev->acc_address, LSM303DLHC_REG_CTRL3_A, LSM303DLHC_CTRL3_A_I1_DRDY1);
    gpio_init(dev->acc_pin, GPIO_IN);

    tmp = LSM303DLHC_TEMP_EN | LSM303DLHC_TEMP_SAMPLE_75HZ;
    res += i2c_write_reg(dev->i2c, dev->mag_address, LSM303DLHC_REG_CRA_M, tmp);

    res += i2c_write_reg(dev->i2c, dev->mag_address,
                        LSM303DLHC_REG_CRB_M, LSM303DLHC_GAIN_5);

    res += i2c_write_reg(dev->i2c, dev->mag_address,
                        LSM303DLHC_REG_MR_M, LSM303DLHC_MAG_MODE_CONTINUOUS);
    i2c_release(dev->i2c);

    gpio_init(dev->mag_pin, GPIO_IN);

    return (res < 6) ? -1 : 0;
}
