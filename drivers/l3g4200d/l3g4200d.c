/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3g4200d
 * @{
 *
 * @file
 * @brief       Device driver implementation for the L3G4200D gyroscope
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>

#include "l3g4200d.h"
#include "l3g4200d-regs.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define MAX_VAL         0x7fff

#define DEV_I2C         (dev->params.i2c)
#define DEV_ADDR        (dev->params.addr)
#define DEV_MODE        (dev->params.mode)
#define DEV_SCALE       (dev->params.scale)

int l3g4200d_init(l3g4200d_t *dev, const l3g4200d_params_t *params)
{
    dev->params = *params;

    uint8_t tmp;

    /* set scale */
    switch (DEV_SCALE) {
        case L3G4200D_SCALE_250DPS:
            dev->scale = 250;
            break;
        case L3G4200D_SCALE_500DPS:
            dev->scale = 500;
            break;
        case L3G4200D_SCALE_2000DPS:
            dev->scale = 2000;
            break;
        default:
            dev->scale = 500;
            break;
    }

    /* acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);

    /* configure CTRL_REG1 */
    tmp = ((DEV_MODE & 0xf) << L3G4200D_CTRL1_MODE_POS) | L3G4200D_CTRL1_ALLON;
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    tmp = ((DEV_SCALE & 0x3) << L3G4200D_CTRL4_FS_POS) | L3G4200D_CTRL4_BDU;
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL4, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    i2c_release(DEV_I2C);
    return 0;
}

int l3g4200d_read(const l3g4200d_t *dev, l3g4200d_data_t *data)
{
    uint8_t tmp[6];
    int16_t res;

    i2c_acquire(DEV_I2C);
    /* get acceleration in x direction */
    i2c_read_regs(DEV_I2C, DEV_ADDR, L3G4200D_REG_OUT_X_L | L3G4200D_AUTOINC, tmp, 6, 0);
    i2c_release(DEV_I2C);

    /* parse and normalize data into result vector */
    res = (tmp[1] << 8) | tmp[0];
    data->acc_x = (int16_t)((dev->scale * res) / MAX_VAL);
    res = (tmp[3] << 8) | tmp[2];
    data->acc_y = (int16_t)((dev->scale * res) / MAX_VAL);
    res = (tmp[5] << 8) | tmp[4];
    data->acc_z = (int16_t)((dev->scale * res) / MAX_VAL);
    return 0;
}

int l3g4200d_enable(const l3g4200d_t *dev)
{
    uint8_t tmp;
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL1, &tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return res;
    }
    tmp |= L3G4200D_CTRL1_PD;
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    i2c_release(DEV_I2C);
    return 0;
}

int l3g4200d_disable(const l3g4200d_t *dev)
{
    uint8_t tmp;
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL1, &tmp, 0);
    if (res < 0) {
        i2c_release(DEV_I2C);
        return res;
    }
    tmp &= ~L3G4200D_CTRL1_PD;
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, L3G4200D_REG_CTRL1, tmp, 0) < 0) {
        i2c_release(DEV_I2C);
        return -1;
    }
    i2c_release(DEV_I2C);
    return 0;
}
