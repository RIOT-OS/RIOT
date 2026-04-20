/*
 * Copyright (C) 2023 LIG
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis2de12
 * @{
 *
 * @file
 * @brief       Device driver implementation for the lis2de12
 *
 * @author      Léo <leo.cordier@etu.esisar.grenoble-inp.fr>
 *
 * @}
 */

#include "assert.h"
#include "byteorder.h"
#include "mutex.h"
#include "timex.h"
#include "ztimer.h"

#include "lis2de12.h"
#include "lis2de12_params.h"
#include "lis2de12_internal.h"
#include "lis2de12_registers.h"



#define ENABLE_DEBUG    0
#include "debug.h"

static const uint16_t hz_per_dr[] = {
    0,      /* power down */
    1,      /* Hz */
    10,     /* Hz */
    25,     /* Hz */
    50,     /* Hz */
    100,    /* Hz */
    200,    /* Hz */
    400,    /* Hz */
    1620,   /* Hz */
    5376,   /* Hz */
};

//I2C functions

/* flag for enabling address auto-incrementation */
#define FLAG_AINC           (0x80)

static int _acquire(const lis2de12_t *dev)
{
    i2c_acquire(dev->params->i2c);
    return 0;
}

static void _release(const lis2de12_t *dev)
{
    i2c_release(dev->params->i2c);
}

static uint8_t _read(const lis2de12_t *dev, uint8_t reg)
{
    uint8_t tmp;
    i2c_read_reg(dev->params->i2c, dev->params->addr, reg, &tmp, 0);
    return tmp;
}

static void _read_burst(const lis2de12_t *dev, uint8_t reg, void *data, size_t len)
{
    i2c_read_regs(dev->params->i2c, dev->params->addr, (FLAG_AINC | reg), data, len, 0);
}

static void _write(const lis2de12_t *dev, uint8_t reg, uint8_t data)
{
    DEBUG("[lis2de12] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    i2c_write_reg(dev->params->i2c, dev->params->addr, reg, data, 0);
}

static void _get_fifo_data(const lis2de12_t *dev, lis2de12_fifo_data_t *dst)
{
    dst->data[0] = (int16_t)((int8_t)_read(dev,REG_OUT_X_H));
    dst->data[1] = (int16_t)((int8_t)_read(dev,REG_OUT_Y_H));
    dst->data[2] = (int16_t)((int8_t)_read(dev,REG_OUT_Z_H));

}

int lis2de12_init(lis2de12_t *dev, const lis2de12_params_t *params)
{
    assert(dev && params);

    dev->params = params;

    /* clear stale data */
    lis2de12_clear_data(dev);

    /* set data range */
    lis2de12_set_scale(dev, (dev->scale));

    /* acquire the bus and verify that our parameters are valid */
    if (_acquire(dev) != 0) {
        DEBUG("[lis2de12] error: unable to acquire the bus\n");
        return LIS2DE12_NOBUS;
    }

    /* read the WHO_IM_I register to verify the connections to the device */
    if (_read(dev, REG_WHO_AM_I) != WHO_AM_I_VAL) {
        DEBUG("[lis2de12] error: invalid value read from WHO_AM_I register\n");
        _release(dev);
        return LIS2DE12_NODEV;
    }

    /* clear events */
    _write(dev, REG_CTRL_REG3, 0);
    _write(dev, REG_CTRL_REG6, 0);

    /* disable fifo */
    _write(dev, REG_FIFO_CTRL_REG, 0);

    /* enable all axes, set sampling rate and scale */
    LIS2DE12_CTRL_REG1_t reg1 = {0};

    reg1.bit.ODR = dev->rate;
    reg1.bit.Xen = 1;
    reg1.bit.Yen = 1;
    reg1.bit.Zen = 1;

    _write(dev, REG_CTRL_REG1, reg1.reg);

    /* enable block data update */
    _write(dev, REG_CTRL_REG4, 0x80);

    _release(dev);

    DEBUG("[lis2de12] initialization successful\n");
    return LIS2DE12_OK;
}

int lis2de12_read(const lis2de12_t *dev, lis2de12_fifo_data_t *data)
{
    assert(dev);

    /* read sampled data from the device */
    _acquire(dev);

    float sensitivity = (((_read(dev, REG_CTRL_REG4) >> 4) & 0x3) + 1) * 15.6f;      //0: +-2g
                                                                                     //1: +-4g
                                                                                     //2: +-8g
                                                                                     //3: +-16g



    // uint8_t comp = ((_read(dev, REG_CTRL_REG4) >> 4) & 0x3) + 2;

    /* first check if valid data is available */
    if ((_read(dev, REG_STATUS_REG) & LIS2DE12_STATUS_REG_ZYXDA) == 0) {
        _release(dev);
        return LIS2DE12_NODATA;
    }

    ;

    _get_fifo_data(dev, data); //get raw values

    //convert to milli-g
    data->axis.x *= sensitivity;
    data->axis.y *= sensitivity;
    data->axis.z *= sensitivity;



    _release(dev);

    return LIS2DE12_OK;
}

int lis2de12_clear_data(const lis2de12_t *dev)
{

    assert(dev);

    LIS2DE12_CTRL_REG5_t ctrl_reg5 = {
        .bit.BOOT = 1,
        .bit.LIR_INT1 = 1,
        .bit.LIR_INT2 = 1,
    };

    _acquire(dev);
    _write(dev, REG_CTRL_REG5, ctrl_reg5.reg);
    _release(dev);

    return LIS2DE12_OK;
}

int lis2de12_set_datarate(const lis2de12_t *dev, lis2de12_rate_t rate)
{

    assert(dev);
    assert(rate <= 0x9);

    LIS2DE12_CTRL_REG1_t reg1;

    _acquire(dev);
    reg1.reg = _read(dev, REG_CTRL_REG1);
    reg1.bit.ODR = rate;
    _write(dev, REG_CTRL_REG1, reg1.reg);
    _release(dev);

    return LIS2DE12_OK;
}

uint16_t lis2de12_get_datarate(const lis2de12_t *dev)
{
    const uint16_t rates_hz[] = {
        0,
        1,
        10,
        25,
        50,
        100,
        200,
        400,
    };

    assert(dev);

    LIS2DE12_CTRL_REG1_t reg1;

    _acquire(dev);
    reg1.reg = _read(dev, REG_CTRL_REG1);
    _release(dev);

    if (reg1.bit.ODR < ARRAY_SIZE(rates_hz)) {
        return rates_hz[reg1.bit.ODR];
    }

    if (reg1.bit.LPen) {
        if (reg1.bit.ODR == 8) {
            return 1620;
        }
        if (reg1.bit.ODR == 9) {
            return 5376;
        }
    }

    if (reg1.bit.ODR == 9) {
        return 1344;
    }

    return 0;
}

int lis2de12_set_scale(lis2de12_t *dev, lis2de12_scale_t scale)
{

    assert(dev);
    assert(scale <= LIS2DE12_SCALE_16G);

    LIS2DE12_CTRL_REG4_t reg4;

    _acquire(dev);
    reg4.reg = _read(dev, REG_CTRL_REG4);
    reg4.bit.FS = scale;
    _write(dev, REG_CTRL_REG4, reg4.reg);
    _release(dev);

    return LIS2DE12_OK;
}

lis2de12_scale_t lis2de12_get_scale(lis2de12_t *dev)
{
    assert(dev);

    LIS2DE12_CTRL_REG4_t reg4;

    _acquire(dev);
    reg4.reg = _read(dev, REG_CTRL_REG4);
    _release(dev);

    return reg4.bit.FS;
}

int lis2de12_set_fifo(const lis2de12_t *dev, const lis2de12_fifo_t *config)
{

    assert(dev && config);

    LIS2DE12_CTRL_REG5_t reg5 = {0};
    LIS2DE12_FIFO_CTRL_REG_t fifo_reg = {0};

    reg5.bit.LIR_INT1 = 1;
    reg5.bit.LIR_INT2 = 1;

    if (config->FIFO_mode != LIS2DE12_FIFO_MODE_BYPASS) {
        reg5.bit.FIFO_EN = 1;
    } else {
        reg5.bit.FIFO_EN = 0;
    }
    fifo_reg.bit.TR = config->FIFO_set_INT2;
    fifo_reg.bit.FM = config->FIFO_mode;
    fifo_reg.bit.FTH = config->FIFO_watermark;

    _acquire(dev);
    _write(dev, REG_CTRL_REG5, reg5.reg);
    _write(dev, REG_FIFO_CTRL_REG, fifo_reg.reg);
    _release(dev);

    return LIS2DE12_OK;
}

int lis2de12_read_temperature(const lis2de12_t *dev, int16_t *temp)
{
    uint8_t bytes[2];

    _acquire(dev);

    /* enable temperature sensor */
    if (!_read(dev, REG_TEMP_CFG_REG)) {
        _write(dev, REG_TEMP_CFG_REG, LIS2DE12_TEMP_CFG_REG_ENABLE);
#if IS_USED(MODULE_ZTIMER_MSEC)
        uint8_t odr = _read(dev, REG_CTRL_REG1) >> 4;
        ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC / hz_per_dr[odr]);
#endif
    }

    _read_burst(dev, REG_OUT_TEMP_L, bytes, sizeof(bytes));
    _release(dev);

    *temp =  100 * ((int16_t)bytes[1] + 25);
    *temp += (100 * bytes[0]) >> 8;

  

    return 0;
}

