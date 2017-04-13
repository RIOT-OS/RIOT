/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lsm6ds3
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM6DS3 3D accelerometer
 *              and 3D gyroscope
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */

#include "lsm6ds3.h"
#include "include/lsm6ds3-internal.h"

#define ENABLE_DEBUG       (0)
#include "debug.h"

#define MASK_INT16_MSB     (0x8000)
#define MASK_INT16_NMSB    (0x7FFF)

#define TEMP_SENSI         (16)
#define TEMP_OFFSET        (25)

#define GYR_DIVIDER        (1000)

/**
  * @brief Takes an unsigned value representing a two's complement number
  *        and returns the signed number it represents
  *
  * @param[in] value       Value which should be calculated
  *
  * @return                The new value extracted from the two's complement
  */
static inline int16_t _twos_complement(int16_t value)
{
    /* Check if value is negative */
    if (value & MASK_INT16_MSB) {
        value = ~(value & MASK_INT16_NMSB) + 1;
        return ~(value & MASK_INT16_NMSB);
    }
    else {
        return value;
    }
}

/**
 * @brief Invokes the _twos_complement function 'num' times and
 *        writes the new value to the corresponding address of 'values[num]'
 *
 * @param[in/out] values   Values which should be calculated
 * @param[in] num          Number of values to calculate
 */
static inline void _twos_complements(int16_t *values, int num)
{
    while (num--) {
        values[num] = _twos_complement(values[num]);
    }
}

/**
 * @brief Reads the raw values from the gyroscope axes
 */
static inline void _gyr_raw (lsm6ds3_t *dev, int16_t *raw)
{
    char tmp[2] = {0,0};

    i2c_acquire(dev->i2c);

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_X_L_G, &tmp[0], 2);
    raw[0] = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_Y_L_G, &tmp[0], 2);
    raw[1] = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_Z_L_G, &tmp[0], 2);
    raw[2] = (tmp[1] << 8) | tmp[0];

    i2c_release(dev->i2c);

    _twos_complements(raw, 3);

    DEBUG("[ G RAW ]\t X: %5.i\tY: %5.i\tZ: %5.i\n", raw[0], raw[1], raw[2]);
}

/**
 * @brief Reads the raw values from the accelerometer axes
 */
static inline void _acc_raw (lsm6ds3_t *dev, int16_t *raw)
{
    char tmp[2] = {0,0};

    i2c_acquire(dev->i2c);

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_X_L_XL, &tmp[0], 2);
    raw[0] = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_Y_L_XL, &tmp[0], 2);
    raw[1] = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_Z_L_XL, &tmp[0], 2);
    raw[2] = (tmp[1] << 8) | tmp[0];

    i2c_release(dev->i2c);

    _twos_complements(raw, 3);

    DEBUG("[ XL RAW ]\t X: %5.i\tY: %5.i\tZ: %5.i\n", raw[0], raw[1], raw[2]);
}

int lsm6ds3_init(lsm6ds3_t *dev,
                 i2c_t i2c,
                 uint8_t address,
                 lsm6ds3_acc_sample_rate_t acc_odr,
                 lsm6ds3_acc_scale_t acc_scale,
                 lsm6ds3_gyr_sample_rate_t gyr_odr,
                 lsm6ds3_gyr_scale_t gyr_scale)
{
    char tmp;

    dev->i2c = i2c;
    dev->addr = address;

    i2c_acquire(dev->i2c);

    i2c_init_master(i2c, I2C_SPEED_NORMAL);

    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_WHO_AM_I_REG, &tmp);

    if (tmp != LSM6DS_CHIP_ID) {
        DEBUG("LIS3MDL: Failed to identify sensor\n");
        return -1;
    }

    tmp = ( LSM6DS3_CTRL9_XL_X_EN      /**< enable x-axis on accelerometer */
          | LSM6DS3_CTRL9_XL_Y_EN      /**< enable y-axis on accelerometer */
          | LSM6DS3_CTRL9_XL_Z_EN);    /**< enable z-axis on accelerometer */
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL9_XL, tmp);

    tmp = ( acc_odr                    /**< accelerometer ODR */
          | acc_scale);                /**< accelerometer FS */
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, tmp);

    tmp = ( LSM6DS3_CTRL10_G_X_EN      /**< enable x-axis on gyroscope */
          | LSM6DS3_CTRL10_G_Y_EN      /**< enable y-axis on gyroscope */
          | LSM6DS3_CTRL10_G_Z_EN);    /**< enable z-axis on gyroscope */
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL10_C, tmp);

    tmp = ( gyr_odr                    /**< gyroscope ODR */
          | gyr_scale );               /**< gyroscope FS */
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, tmp);

    i2c_release(dev->i2c);

    return 0;
}

void lsm6ds3_read_acc(lsm6ds3_t *dev, lsm6ds3_3d_data_t *data)
{
    char tmp;
    float sensitivity = 0;
    int16_t raw[3];    /**< raw[0] := X-, raw[1] := Y-, raw[2] := Z-axis */

    _acc_raw(dev, raw);

    i2c_acquire(dev->i2c);
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, &tmp);
    i2c_release(dev->i2c);

    tmp &= LSM6DS3_MASK_CTRL1_XL_FS;

    switch (tmp) {

        case LSM6DS3_MASK_CTRL1_XL_FS_2G:
            sensitivity = 0.061;
            break;

        case LSM6DS3_MASK_CTRL1_XL_FS_4G:
            sensitivity = 0.122;
            break;

        case LSM6DS3_MASK_CTRL1_XL_FS_8G:
            sensitivity = 0.244;
            break;

        case LSM6DS3_MASK_CTRL1_XL_FS_16G:
            sensitivity = 0.488;
            break;
    }

    data->x_axis = raw[0] * sensitivity;
    data->y_axis = raw[1] * sensitivity;
    data->z_axis = raw[2] * sensitivity;
}

void lsm6ds3_read_gyr(lsm6ds3_t *dev, lsm6ds3_3d_data_t *data)
{
    char tmp;
    float sensitivity = 0;
    int16_t raw[3];    /**< raw[0] := X-, raw[1] := Y-, raw[2] := Z-axis */

    _gyr_raw(dev, raw);

    i2c_acquire(dev->i2c);
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, &tmp);
    i2c_release(dev->i2c);

    tmp &= LSM6DS3_MASK_CTRL2_G_FS;

    switch (tmp) {

        case LSM6DS3_MASK_CTRL2_G_FS_125DPS:
            sensitivity = 4.375;
            break;

        case LSM6DS3_MASK_CTRL2_G_FS_245DPS:
            sensitivity = 8.75;
            break;

        case LSM6DS3_MASK_CTRL2_G_FS_500DPS:
            sensitivity = 17.50;
            break;

        case LSM6DS3_MASK_CTRL2_G_FS_1000DPS:
            sensitivity = 35;
            break;

        case LSM6DS3_MASK_CTRL2_G_FS_2000DPS:
            sensitivity = 7;
            break;
    }

    data->x_axis = raw[0] * sensitivity / GYR_DIVIDER;
    data->y_axis = raw[1] * sensitivity / GYR_DIVIDER;
    data->z_axis = raw[2] * sensitivity / GYR_DIVIDER;
}

void lsm6ds3_read_temp(lsm6ds3_t *dev, int16_t *data)
{
    i2c_acquire(dev->i2c);
    i2c_read_regs(dev->i2c, dev->addr, LSM6DS3_OUT_TEMP_L, (char*)value, 2);
    i2c_release(dev->i2c);

    *value = _twos_complement(*value);

    DEBUG("[ TEMP RAW ]\t°C: %5.i\n", *value);

    *value = (TEMP_OFFSET + (*value / TEMP_SENSI));
}

void lsm6ds3_enable(lsm6ds3_t *dev)
{
    char tmp;

    i2c_acquire(dev->i2c);

    /* Accelerometer normal power mode */
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, &tmp);
    tmp &= !(LSM6DS3_MASK_CTRL1_XL_ODR);
    tmp |= (LSM6DS3_MASK_CTRL1_XL_ODR_208HZ);
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, tmp);

    /* Gyroscope normal power mode */
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, &tmp);
    tmp &= !(LSM6DS3_MASK_CTRL2_G_ODR);
    tmp |= (LSM6DS3_MASK_CTRL2_G_ODR_208HZ);
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, tmp);

    i2c_release(dev->i2c);
}

void lsm6ds3_disable(lsm6ds3_t *dev)
{
    char tmp;

    i2c_acquire(dev->i2c);

    /* Accelerometer power down mode */
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, &tmp);
    tmp &= !(LSM6DS3_MASK_CTRL1_XL_ODR);
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL1_XL, tmp);

    /* Gyroscope power down mode */
    i2c_read_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, &tmp);
    tmp &= !(LSM6DS3_MASK_CTRL2_G_ODR);
    i2c_write_reg(dev->i2c, dev->addr, LSM6DS3_CTRL2_G, tmp);

    i2c_release(dev->i2c);
}
