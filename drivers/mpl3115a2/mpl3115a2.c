/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mpl3115a2
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MPL3115A2 Sensor.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"
#include "mpl3115a2.h"
#include "mpl3115a2_reg.h"

#include "debug.h"

#define I2C_SPEED                  I2C_SPEED_FAST

int mpl3115a2_test(mpl3115a2_t *dev)
{
    char reg;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_WHO_AM_I, &reg, 1) != 1) {
        /* Release the bus for other threads. */
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    if (reg != MPL3115A2_ID) {
        return -1;
    }

    return 0;
}

int mpl3115a2_init(mpl3115a2_t *dev, i2c_t i2c, uint8_t address, uint8_t os_ratio)
{
    char reg;

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    if (os_ratio > MPL3115A2_OS_RATIO_128) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        i2c_release(dev->i2c);
        return -2;
    }
    i2c_release(dev->i2c);

    if (mpl3115a2_test(dev)) {
        return -3;
    }

    reg = MPL3115A2_CTRL_REG1_OS(os_ratio);

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }
    i2c_release(dev->i2c);

    reg = MPL3115A2_PT_DATA_CFG_TDEFE
          | MPL3115A2_PT_DATA_CFG_PDEFE
          | MPL3115A2_PT_DATA_CFG_DREM;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MPL3115A2_PT_DATA_CFG, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -4;
    }

    i2c_release(dev->i2c);
    dev->initialized = true;

    return 0;
}

int mpl3115a2_reset(mpl3115a2_t *dev)
{
    char reg;

    dev->initialized = false;
    reg = MPL3115A2_CTRL_REG1_RST;

    i2c_acquire(dev->i2c);
    if (i2c_write_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return 0;
}

int mpl3115a2_set_active(mpl3115a2_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg |= MPL3115A2_CTRL_REG1_SBYB;

    if (i2c_write_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return 0;
}

int mpl3115a2_set_standby(mpl3115a2_t *dev)
{
    char reg;

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    reg &= ~MPL3115A2_CTRL_REG1_SBYB;

    if (i2c_write_regs(dev->i2c, dev->addr, MPL3115A2_CTRL_REG1, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return 0;
}

int mpl3115a2_is_ready(mpl3115a2_t *dev)
{
    char reg;

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_STATUS, &reg, 1) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);
    return reg & MPL3115A2_STATUS_PTDR;
}

int mpl3115a2_read_pressure(mpl3115a2_t *dev, uint32_t *pres, uint8_t *status)
{
    char buf[4];

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_STATUS, buf, 4) != 4) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    *status = buf[0];

    *pres = ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];
    *pres = *pres / 64;

    return 0;
}

int mpl3115a2_read_temp(mpl3115a2_t *dev, int16_t *temp)
{
    char buf[2];

    if (dev->initialized == false) {
        return -1;
    }

    i2c_acquire(dev->i2c);
    if (i2c_read_regs(dev->i2c, dev->addr, MPL3115A2_OUT_T_MSB, buf, 2) != 2) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    *temp = ((int16_t)(((int16_t)buf[0] << 8) | buf[1]) * 10) / 256;

    return 0;
}
