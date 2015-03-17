#/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_l3g4200d
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

#include "debug.h"

#define I2C_SPEED       I2C_SPEED_FAST

#define MAX_VAL         0x7fff

int l3g4200d_init(l3g4200d_t *dev, i2c_t i2c, uint8_t address,
                  gpio_t int1_pin, gpio_t int2_pin,
                  l3g4200d_mode_t mode, l3g4200d_scale_t scale)
{
    char tmp;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->int1 = int1_pin;
    dev->int2 = int2_pin;

    /* set scale */
    switch (scale) {
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

    /* configure CTRL_REG1 */
    tmp = ((mode & 0xf) << L3G4200D_CTRL1_MODE_POS) | L3G4200D_CTRL1_ALLON;
    i2c_acquire(dev->i2c);
    if (i2c_write_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL1, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    tmp = ((scale & 0x3) << L3G4200D_CTRL4_FS_POS) | L3G4200D_CTRL4_BDU;
    if (i2c_write_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL4, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    return 0;
}

int l3g4200d_read(l3g4200d_t *dev, l3g4200d_data_t *data)
{
    char tmp[6];
    int16_t res;

    i2c_acquire(dev->i2c);
    /* get acceleration in x direction */
    i2c_read_regs(dev->i2c, dev->addr, L3G4200D_REG_OUT_X_L | L3G4200D_AUTOINC, tmp, 6);
    i2c_release(dev->i2c);

    /* parse and normalize data into result vector */
    res = (tmp[1] << 8) | tmp[0];
    data->acc_x = (int16_t)((dev->scale * res) / MAX_VAL);
    res = (tmp[3] << 8) | tmp[2];
    data->acc_y = (int16_t)((dev->scale * res) / MAX_VAL);
    res = (tmp[5] << 8) | tmp[4];
    data->acc_z = (int16_t)((dev->scale * res) / MAX_VAL);
    return 0;
}

int l3g4200d_enable(l3g4200d_t *dev)
{
    char tmp;
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_read_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL1, &tmp);
    if (res < 1) {
        i2c_release(dev->i2c);
        return res;
    }
    tmp |= L3G4200D_CTRL1_PD;
    if (i2c_write_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL1, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    return 0;
}

int l3g4200d_disable(l3g4200d_t *dev)
{
    char tmp;
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_read_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL1, &tmp);
    if (res < 1) {
        i2c_release(dev->i2c);
        return res;
    }
    tmp &= ~L3G4200D_CTRL1_PD;
    if (i2c_write_reg(dev->i2c, dev->addr, L3G4200D_REG_CTRL1, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);
    return 0;
}
