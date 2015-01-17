/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "periph/i2c.h"
#include "tmp006.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TMP006_V_OBJECT            0x00 /**< Sensor Voltage Register */
#define TMP006_T_AMBIENT           0x01 /**< Ambient Temperature Register */
#define TMP006_CONFIG              0x02 /**< Configuration Register */
#define TMP006_MANUFACTURER_ID     0xFE /**< Manufacturer ID Register */
#define TMP006_DEVICE_ID           0xFF /**< Device ID Register */

#define TMP006_CONFIG_RST          (1 << 15)

#define TMP006_CONFIG_MOD_SHIFT    12
#define TMP006_CONFIG_MOD_MASK     0x7000
#define TMP006_CONFIG_MOD(x)       (((uint16_t)(((uint16_t)(x))<<TMP006_CONFIG_MOD_SHIFT))\
                                   &TMP006_CONFIG_MOD_MASK)
#define TMP006_CONFIG_MOD_CC       0x07
#define TMP006_CONFIG_MOD_OFF      0x00

#define TMP006_CONFIG_CR_SHIFT     9
#define TMP006_CONFIG_CR_MASK      0x0E00
#define TMP006_CONFIG_CR(x)        (((uint16_t)(((uint16_t)(x))<<TMP006_CONFIG_CR_SHIFT))\
                                   &TMP006_CONFIG_CR_MASK)

#define TMP006_CONFIG_DRDY_PIN_EN  (1 << 8)
#define TMP006_CONFIG_DRDY         (1 << 7)

#define TMP006_MID_VALUE           0x5449 /**< Manufacturer ID */
#define TMP006_DID_VALUE           0x0067 /**< Device ID */

#define I2C_SPEED                  I2C_SPEED_FAST

int tmp006_test(tmp006_t *dev)
{
    char reg[2];
    uint16_t tmp;

    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_DEVICE_ID, reg, 2) != 2) {
        return -1;
    }

    tmp = ((uint16_t)reg[0] << 8) | reg[1];

    if (tmp != TMP006_DID_VALUE) {
        return -1;
    }

    return 0;
}

int tmp006_init(tmp006_t *dev, i2c_t i2c, uint8_t address, uint8_t conv_rate)
{
    char reg[2];

    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    dev->initialized = false;

    if (conv_rate > TMP006_CONFIG_CR_AS16) {
        return -1;
    }

    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        return -2;
    }

    if (tmp006_test(dev)) {
        return -3;
    }

    uint16_t tmp = TMP006_CONFIG_CR(conv_rate);
    reg[0] = (uint8_t)(tmp >> 8);
    reg[1] = (uint8_t)tmp;

    if (i2c_write_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -4;
    }

    dev->initialized = true;

    return 0;
}

int tmp006_reset(tmp006_t *dev)
{
    char reg[2];
    uint16_t tmp = TMP006_CONFIG_RST;
    reg[0] = (uint8_t)(tmp >> 8);
    reg[1] = (uint8_t)tmp;
    dev->initialized = false;

    if (i2c_write_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -1;
    }

    return 0;
}

int tmp006_set_active(tmp006_t *dev)
{
    char reg[2];

    if (dev->initialized == false) {
        return -1;
    }

    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -1;
    }

    reg[0] |= (uint8_t)(TMP006_CONFIG_MOD(TMP006_CONFIG_MOD_CC) >> 8);

    if (i2c_write_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -1;
    }

    return 0;
}

int tmp006_set_standby(tmp006_t *dev)
{
    char reg[2];

    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -1;
    }

    reg[0] &= ~(uint8_t)(TMP006_CONFIG_MOD(TMP006_CONFIG_MOD_CC) >> 8);

    if (i2c_write_regs(dev->i2c, dev->addr, TMP006_CONFIG, reg, 2) != 2) {
        return -1;
    }

    return 0;
}

int tmp006_read(tmp006_t *dev, int16_t *rawv, int16_t *rawt, uint8_t *drdy)
{
    char buf[2];

    if (dev->initialized == false) {
        return -1;
    }

    /* Register bytes are sent MSB first. */
    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_CONFIG, buf, 2) != 2) {
        return -1;
    }

    *drdy = buf[1] & (uint8_t)(TMP006_CONFIG_DRDY);

    if (!(*drdy)) {
        /* conversion in progress */
        return -1;
    }

    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_V_OBJECT, buf, 2) != 2) {
        return -1;
    }

    *rawv = ((uint16_t)buf[0] << 8) | buf[1];

    if (i2c_read_regs(dev->i2c, dev->addr, TMP006_T_AMBIENT, buf, 2) != 2) {
        return -1;
    }

    *rawt = ((uint16_t)buf[0] << 8) | buf[1];

    return 0;
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
