/*
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "periph/i2c.h"

#include "mpl3115a2.h"
#include "mpl3115a2_reg.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define BUS             (dev->params.i2c)
#define ADDR            (dev->params.addr)

int mpl3115a2_init(mpl3115a2_t *dev, const mpl3115a2_params_t *params)
{
    uint8_t reg;

    assert(dev);
    assert(params);

    /* write device descriptor */
    dev->params = *params;

    i2c_acquire(BUS);
    /* test device */
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_WHO_AM_I, &reg, 1, 0) < 0) {
        /* Release the bus for other threads. */
        i2c_release(BUS);
        LOG_ERROR("mpl3115a2_init: I2C error!\n");
        return -MPL3115A2_ERROR_I2C;
    }
    if (reg != MPL3115A2_ID) {
        LOG_ERROR("mpl3115a2_init: invalid WHO_AM_I value (0x%02x)!\n", (int)reg);
        return -MPL3115A2_ERROR_DEV;
    }
    /* set sample rate */
    reg = MPL3115A2_CTRL_REG1_OS(dev->params.ratio);
    if (i2c_write_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mpl3115a2_init: failed to set sample rate!\n");
        return -MPL3115A2_ERROR_CNF;
    }
    /* configure device */
    reg = MPL3115A2_PT_DATA_CFG_TDEFE |
          MPL3115A2_PT_DATA_CFG_PDEFE |
          MPL3115A2_PT_DATA_CFG_DREM;
    if (i2c_write_regs(BUS, ADDR, MPL3115A2_PT_DATA_CFG, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mpl3115a2_init: config failure!\n");
        return -MPL3115A2_ERROR_CNF;
    }

    i2c_release(BUS);

    return MPL3115A2_OK;
}

int mpl3115a2_reset(const mpl3115a2_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    reg = MPL3115A2_CTRL_REG1_RST;
    if (i2c_write_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("mpl3115a2_reset: failed!\n");
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    return MPL3115A2_OK;
}

int mpl3115a2_set_active(const mpl3115a2_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }

    reg |= MPL3115A2_CTRL_REG1_SBYB;
    if (i2c_write_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    return MPL3115A2_OK;
}

int mpl3115a2_set_standby(const mpl3115a2_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }

    reg &= ~MPL3115A2_CTRL_REG1_SBYB;
    if (i2c_write_regs(BUS, ADDR, MPL3115A2_CTRL_REG1, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    return MPL3115A2_OK;
}

int mpl3115a2_is_ready(const mpl3115a2_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_STATUS, &reg, 1, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    return reg & MPL3115A2_STATUS_PTDR;
}

int mpl3115a2_read_pressure(const mpl3115a2_t *dev, uint32_t *pres, uint8_t *status)
{
    uint8_t buf[4];

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_STATUS, buf, 4, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    *status = buf[0];

    *pres = ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];
    *pres = *pres / 64;

    return MPL3115A2_OK;
}

int mpl3115a2_read_temp(const mpl3115a2_t *dev, int16_t *temp)
{
    uint8_t buf[2];

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, MPL3115A2_OUT_T_MSB, buf, 2, 0) < 0) {
        i2c_release(BUS);
        return -MPL3115A2_ERROR_I2C;
    }
    i2c_release(BUS);

    *temp = ((int16_t)(((int16_t)buf[0] << 8) | buf[1]) * 10) / 256;

    return MPL3115A2_OK;
}
