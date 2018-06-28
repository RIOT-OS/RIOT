/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_tmp006
 * @{
 *
 * @file
 * @brief       Driver for the TI TMP006 Infrared Thermopile Sensor.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "periph/i2c.h"
#include "tmp006.h"
#include "tmp006_regs.h"
#if TMP006_USE_LOW_POWER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG                (0)
#include "debug.h"

#define TMP006_CONFIG_RST           (1 << 15)

#define TMP006_CONFIG_MOD_SHIFT     (12U)
#define TMP006_CONFIG_MOD_MASK      (0x7000)
#define TMP006_CONFIG_MOD(x)        (((uint16_t)(((uint16_t)(x)) << TMP006_CONFIG_MOD_SHIFT))\
                                     & TMP006_CONFIG_MOD_MASK)
#define TMP006_CONFIG_MOD_CC        (0x07)
#define TMP006_CONFIG_MOD_OFF       (0x00)

#define TMP006_CONFIG_CR_SHIFT      (9U)
#define TMP006_CONFIG_CR_MASK       (0x0E00)
#define TMP006_CONFIG_CR(x)         (((uint16_t)(((uint16_t)(x)) << TMP006_CONFIG_CR_SHIFT))\
                                     & TMP006_CONFIG_CR_MASK)

#define TMP006_CONFIG_DRDY_PIN_EN   (1 << 8)
#define TMP006_CONFIG_DRDY          (1 << 7)

#define TMP006_MID_VALUE            (0x5449) /**< Manufacturer ID */
#define TMP006_DID_VALUE            (0x0067) /**< Device ID */

#define BUS                         (dev->p.i2c)
#define ADDR                        (dev->p.addr)

int tmp006_init(tmp006_t *dev, const tmp006_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    uint8_t reg[2];
    uint16_t tmp;

    /* initialize the device descriptor */
    memcpy(&dev->p, params, sizeof(tmp006_params_t));

    if (dev->p.rate > TMP006_CONFIG_CR_AS16) {
        LOG_ERROR("tmp006_init: invalid conversion rate!\n");
        return -TMP006_ERROR_CONF;
    }

    /* test device id */
    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_DEVICE_ID, reg, 2, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("tmp006_init: error reading device ID!\n");
        return -TMP006_ERROR_BUS;
    }
    tmp = ((uint16_t)reg[0] << 8) | reg[1];
    if (tmp != TMP006_DID_VALUE) {
        return -TMP006_ERROR_DEV;
    }
    /* set conversion rate */
    tmp = TMP006_CONFIG_CR(dev->p.rate);
    reg[0] = (tmp >> 8);
    reg[1] = tmp;
    if (i2c_write_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        LOG_ERROR("tmp006_init: error setting conversion rate!\n");
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);

    return TMP006_OK;
}

int tmp006_reset(const tmp006_t *dev)
{
    uint8_t reg[2];
    uint16_t tmp = TMP006_CONFIG_RST;
    reg[0] = (tmp >> 8);
    reg[1] = tmp;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(BUS);
    if (i2c_write_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);
    return TMP006_OK;
}

int tmp006_set_active(const tmp006_t *dev)
{
    uint8_t reg[2];

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }

    reg[0] |= (TMP006_CONFIG_MOD(TMP006_CONFIG_MOD_CC) >> 8);
    if (i2c_write_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);
    return TMP006_OK;
}

int tmp006_set_standby(const tmp006_t *dev)
{
    uint8_t reg[2];

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }

    reg[0] &= ~(TMP006_CONFIG_MOD(TMP006_CONFIG_MOD_CC) >> 8);
    if (i2c_write_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);
    return TMP006_OK;
}

int tmp006_read(const tmp006_t *dev, int16_t *rawv, int16_t *rawt, uint8_t *drdy)
{
    uint8_t reg[2];

    i2c_acquire(BUS);
    /* Register bytes are sent MSB first. */
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_CONFIG, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);

    *drdy = reg[1] & (TMP006_CONFIG_DRDY);
    if (!(*drdy)) {
        LOG_DEBUG("tmp006_read: conversion in progress!\n");
        return -TMP006_ERROR;
    }

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_V_OBJECT, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);

    *rawv = ((uint16_t)reg[0] << 8) | reg[1];

    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR, TMP006_REGS_T_AMBIENT, reg, 2, 0) < 0) {
        i2c_release(BUS);
        return -TMP006_ERROR_BUS;
    }
    i2c_release(BUS);
    *rawt = ((uint16_t)reg[0] << 8) | reg[1];
    return TMP006_OK;
}

void tmp006_convert(int16_t rawv, int16_t rawt,  float *tamb, float *tobj)
{
    /* calculate die temperature */
    *tamb = (double)rawt / 128.0;
    /* die temperature in Kelvin */
    double tdie_k = *tamb + 273.15;

    /* calculate sensor voltage */
    double sens_v = (double)rawv * TMP006_CCONST_LSB_SIZE;

    double tdiff = tdie_k - TMP006_CCONST_TREF;
    double tdiff_pow2 = pow(tdiff, 2);

    double s = TMP006_CCONST_S0 * (1 + TMP006_CCONST_A1 * tdiff
                                   + TMP006_CCONST_A2 * tdiff_pow2);

    double v_os = TMP006_CCONST_B0 + TMP006_CCONST_B1 * tdiff
                  + TMP006_CCONST_B2 * tdiff_pow2;

    double f_obj = (sens_v - v_os) + TMP006_CCONST_C2 * pow((sens_v - v_os), 2);

    double t = pow(pow(tdie_k, 4) + (f_obj / s), 0.25);
    /* calculate object temperature in Celsius */
    *tobj = (t - 273.15);
}

int tmp006_read_temperature(const tmp006_t *dev, int16_t *ta, int16_t *to)
{

    uint8_t drdy;
#if (!TMP006_USE_RAW_VALUES)
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
#endif

#if TMP006_USE_LOW_POWER
    if (tmp006_set_active(dev)) {
        return TMP006_ERROR;
    }
    xtimer_usleep(TMP006_CONVERSION_TIME);
#endif

#if TMP006_USE_RAW_VALUES
    tmp006_read(dev, to, ta, &drdy);

    if (!drdy) {
        return TMP006_ERROR;
    }
#else
    tmp006_read(dev, &rawvolt, &rawtemp, &drdy);

    if (!drdy) {
        return TMP006_ERROR;
    }
    tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
    *ta = (int16_t)(tamb*100);
    *to = (int16_t)(tobj*100);
#endif

#if TMP006_USE_LOW_POWER
    if (tmp006_set_standby(dev)) {
        return TMP006_ERROR;
    }
#endif

    return TMP006_OK;
}
