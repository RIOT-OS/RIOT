/*
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2017-2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tmp00x
 * @{
 *
 * @file
 * @brief       Driver for the TI TMP00X (TMP006 and TMP007) Infrared Thermopile Sensor.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "periph/i2c.h"
#include "tmp00x.h"
#include "tmp00x_regs.h"
#include "byteorder.h"
#include "kernel_defines.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int tmp00x_init(tmp00x_t *dev, const tmp00x_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    uint16_t reg;

    /* initialize the device descriptor */
    dev->p = *params;

    if (dev->p.rate > TMP00X_CONFIG_CR_AS16) {
        LOG_ERROR("tmp00x_init: invalid conversion rate!\n");
        return -TMP00X_ERROR_CONF;
    }

    /* test device id */
    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_DEVICE_ID, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("tmp00x_init: error reading device ID!\n");
        return -TMP00X_ERROR_BUS;
    }

    reg = htons(reg);
    if (reg != TMP00X_DID_VALUE) {
        return -TMP00X_ERROR_DEV;
    }

    /* set conversion rate */
    reg = TMP00X_CONFIG_CR(dev->p.rate);
    if (i2c_write_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("tmp00x_init: error setting conversion rate!\n");
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);

    return TMP00X_OK;
}

int tmp00x_reset(const tmp00x_t *dev)
{
    uint16_t reg = TMP00X_CONFIG_RST;
    reg = htons(reg);

    /* Acquire exclusive access to the bus. */
    i2c_acquire(BUS);
    if (i2c_write_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);
    return TMP00X_OK;
}

int tmp00x_set_active(const tmp00x_t *dev)
{
    uint16_t reg;

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }

    reg |= (TMP00X_CONFIG_MOD(TMP00X_CONFIG_MOD_CC) >> 8);
    if (i2c_write_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }

    i2c_release(BUS);
    return TMP00X_OK;
}

int tmp00x_set_standby(const tmp00x_t *dev)
{
    uint16_t reg;

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }

    reg &= ~(TMP00X_CONFIG_MOD(TMP00X_CONFIG_MOD_CC) >> 8);
    if (i2c_write_regs(BUS, ADDR, TMP00X_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);
    return TMP00X_OK;
}

int tmp00x_read(const tmp00x_t *dev, int16_t *rawv, int16_t *rawt, uint16_t *drdy)
{
    uint16_t reg;

    i2c_acquire(BUS);
    /* Register bytes are sent MSB first. */
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_READ_STATUS, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);

    *drdy = htons(reg) & (TMP00X_DRDY);

    if (!(*drdy)) {
        LOG_DEBUG("tmp00x_read: conversion in progress!\n");
        return -TMP00X_ERROR;
    }

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_V_OBJECT, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);

    *rawv = htons(reg);

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP00X_REGS_T_AMBIENT, &reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP00X_ERROR_BUS;
    }
    i2c_release(BUS);
    *rawt = htons(reg);
    return TMP00X_OK;
}

void tmp00x_convert(int16_t rawv, int16_t rawt,  float *tamb, float *tobj)
{
    /* calculate die temperature */
    *tamb = (float)rawt / 128.0;
    /* die temperature in Kelvin */
    float tdie_k = *tamb + 273.15;

    /* calculate sensor voltage */
    float sens_v = (float)rawv * TMP00X_CCONST_LSB_SIZE;

    float tdiff = tdie_k - TMP00X_CCONST_TREF;
    float tdiff_pow2 = pow(tdiff, 2);

    float s = TMP00X_CCONST_S0 * (1 + TMP00X_CCONST_A1 * tdiff
                                   + TMP00X_CCONST_A2 * tdiff_pow2);

    float v_os = TMP00X_CCONST_B0 + TMP00X_CCONST_B1 * tdiff
                  + TMP00X_CCONST_B2 * tdiff_pow2;

    float f_obj = (sens_v - v_os) + TMP00X_CCONST_C2 * pow((sens_v - v_os), 2);

    float t = pow(pow(tdie_k, 4) + (f_obj / s), 0.25);
    /* calculate object temperature in Celsius */
    *tobj = (t - 273.15);
}

int tmp00x_read_temperature(const tmp00x_t *dev, int16_t *ta, int16_t *to)
{
    uint16_t drdy;
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
    int ret;

    if (IS_ACTIVE(CONFIG_TMP00X_USE_LOW_POWER)) {
        if (tmp00x_set_active(dev)) {
            return TMP00X_ERROR;
        }
        xtimer_usleep(CONFIG_TMP00X_CONVERSION_TIME);
    }

    if (IS_ACTIVE(CONFIG_TMP00X_USE_RAW_VALUES)) {
        if ((ret = tmp00x_read(dev, to, ta, &drdy)) < 0) {
            return ret;
        }

        if (!drdy) {
            if (IS_ACTIVE(CONFIG_TMP00X_USE_LOW_POWER)) {
                tmp00x_set_standby(dev);
            }
            return -TMP00X_ERROR;
        }
    }
    else {
        if ((ret = tmp00x_read(dev, &rawvolt, &rawtemp, &drdy)) < 0) {
            return ret;
        }

        if (!drdy) {
            if (IS_ACTIVE(CONFIG_TMP00X_USE_LOW_POWER)) {
                tmp00x_set_standby(dev);
            }
            return -TMP00X_ERROR;
        }
        tmp00x_convert(rawvolt, rawtemp,  &tamb, &tobj);
        *ta = (int16_t)(tamb*100);
        *to = (int16_t)(tobj*100);
    }

    if (IS_ACTIVE(CONFIG_TMP00X_USE_LOW_POWER)) {
        if (tmp00x_set_standby(dev)) {
            return -TMP00X_ERROR;
        }
    }
    return TMP00X_OK;
}
