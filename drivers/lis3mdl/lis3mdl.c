/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lis3mdl
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LIS3MDL 3-axis magnetometer
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */

#include "lis3mdl.h"
#include "include/lis3mdl-internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MASK_INT16_MSB     (0x8000)
#define MASK_INT16_NMSB    (0x7FFF)

#define TEMP_DIVIDER       (16)
#define TEMP_OFFSET        (25)

#define GAUSS_DIVIDER      (1000)

/**
 * @brief Takes an unsigned value representing a two's complement number
 *        and returns the signed number it represents
 *
 * @param[in] value    value which represents a two's complement number
 *
 * @return             the converted signed number of 'value'
 */
 static inline int16_t _twos_complement(int16_t value)
{
    if (value & MASK_INT16_MSB) {
        value = ~(value & MASK_INT16_NMSB) + 1;
        return ~(value & MASK_INT16_NMSB);
    }
    else {
        return value;
    }
}

int lis3mdl_init(lis3mdl_t *dev,
                 i2c_t i2c,
                 uint8_t address,
                 lis3mdl_xy_mode_t xy_mode,
                 lis3mdl_z_mode_t z_mode,
                 lis3mdl_odr_t odr,
                 lis3mdl_scale_t scale,
                 lis3mdl_op_t op_mode) {
    char tmp;

    dev->i2c = i2c;
    dev->addr = address;

    i2c_acquire(dev->i2c);

    if (i2c_init_master(i2c, I2C_SPEED_NORMAL) < 0) {
        DEBUG("LIS3MDL: Master initialization failed\n");
        return -1;
    }

    i2c_read_reg(dev->i2c, dev->addr, LIS3DML_WHO_AM_I_REG, &tmp);
    if (tmp != LIS3MDL_CHIP_ID) {
        DEBUG("LIS3MDL: Identification failed\n");
        return -1;
    }

    tmp = ( LIS3MDL_MASK_REG1_TEMP_EN   /* enable temperature sensor */
          | xy_mode                     /* set x-, y-axis operative mode */
          | odr);                       /* set output data rate */
    i2c_write_reg(dev->i2c, dev->addr, LIS3MDL_CTRL_REG1, tmp);

    /* set Full-scale configuration */
    i2c_write_reg(dev->i2c, dev->addr, LIS3MDL_CTRL_REG2, scale);

    /* set continuous-conversion mode */
    i2c_write_reg(dev->i2c, dev->addr, LIS3MDL_CTRL_REG3, op_mode);

    /* set z-axis operative mode */
    i2c_write_reg(dev->i2c, dev->addr, LIS3MDL_CTRL_REG4, z_mode);

    i2c_release(dev->i2c);

    return 0;
}

void lis3mdl_read_mag(lis3mdl_t *dev, lis3mdl_3d_data_t *data)
{
    char tmp[2] = {0, 0};

    i2c_acquire(dev->i2c);

    i2c_read_regs(dev->i2c, dev->addr, LIS3MDL_OUT_X_L_REG, &tmp[0], 2);
    data->x_axis = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LIS3MDL_OUT_Y_L_REG, &tmp[0], 2);
    data->y_axis = (tmp[1] << 8) | tmp[0];

    i2c_read_regs(dev->i2c, dev->addr, LIS3MDL_OUT_Z_L_REG, &tmp[0], 2);
    data->z_axis = (tmp[1] << 8) | tmp[0];

    data->x_axis = _twos_complement(data->x_axis);
    data->y_axis = _twos_complement(data->y_axis);
    data->z_axis = _twos_complement(data->z_axis);

    /* Divide the raw data by 1000 to geht [G] := Gauss */
    data->x_axis /= GAUSS_DIVIDER;
    data->y_axis /= GAUSS_DIVIDER;
    data->z_axis /= GAUSS_DIVIDER;

    i2c_release(dev->i2c);
}

void lis3mdl_read_temp(lis3mdl_t *dev, int16_t *value)
{
    i2c_acquire(dev->i2c);
    i2c_read_regs(dev->i2c, dev->addr, LIS3MDL_TEMP_OUT_L_REG, (char*)value, 2);
    i2c_release(dev->i2c);

    *value = _twos_complement(*value);

    *value = (TEMP_OFFSET + (*value / TEMP_DIVIDER));
}

void lis3mdl_enable(lis3mdl_t *dev)
{
    i2c_acquire(dev->i2c);
    /* Z-axis medium-power mode */
    i2c_write_reg(dev->i2c, dev->addr,
                  LIS3MDL_CTRL_REG3, LIS3MDL_MASK_REG3_Z_MEDIUM_POWER);
    i2c_release(dev->i2c);
}

void lis3mdl_disable(lis3mdl_t *dev)
{
    char tmp = ( LIS3MDL_MASK_REG3_LOW_POWER_EN   /**< enable power-down mode */
               | LIS3MDL_MASK_REG3_Z_LOW_POWER);  /**< Z-axis low-power mode */

    i2c_acquire(dev->i2c);
    i2c_write_reg(dev->i2c, dev->addr, LIS3MDL_CTRL_REG3, tmp);
    i2c_release(dev->i2c);
}
