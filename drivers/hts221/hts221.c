/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_hts221
 * @{
 *
 * @file
 * @brief       Driver for the ST HTS221 digital Humidity Sensor
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "hts221.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define BUS             (dev->p.i2c)
#define ADDR            (dev->p.addr)

static int _set_power(const hts221_t *dev, const bool active);

/**
 * @brief Read calibration parameters for humidity calculation
 *
 * @note This function does not acquire or release the I2C bus, must be done
 *       in calling function!
 *
 * @param[in|out] dev   device descriptor of sensor
 *
 * @returns             HTS221_OK on success, or error otherwise
 */
int _humidity_calibration(hts221_t *dev)
{
    uint8_t buf[2];
    uint8_t reg = HTS221_REGS_H0_RH_X2 | 0x80;
    /* 1. read h0_rh and h1_rh coefficients */
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    dev->h0_rh = buf[0] >> 1;
    dev->h1_rh = buf[1] >> 1;
    DEBUG("%s: h0_rh %" PRIi16 ", h1_rh %" PRIi16 "\n",
          DEBUG_FUNC, dev->h0_rh, dev->h1_rh);
    /* 2. read h0_t0_out */
    reg = HTS221_REGS_H0_T0_OUT_L | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    dev->h0_t0_out = ((uint16_t)buf[1] << 8) | buf[0];
    /* 3. read h1_t0_out */
    reg = HTS221_REGS_H1_T0_OUT_L | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    dev->h1_t0_out = ((uint16_t)buf[1] << 8) | buf[0];
    DEBUG("%s: h0_t0_out %" PRIi16 ", h1_t0_out %" PRIi16 "\n",
          DEBUG_FUNC, dev->h0_t0_out, dev->h1_t0_out);

    return HTS221_OK;
}

/**
 * @brief Read calibration parameters for temperature calculation
 *
 * @note This function does not acquire or release the I2C bus, must be done
 *       in calling function!
 *
 * @param[in|out] dev   device descriptor of sensor
 *
 * @returns             HTS221_OK on success, or error otherwise
 */
int _temperature_calibration(hts221_t *dev)
{
    uint8_t buf[2];
    uint8_t tmp;
    /* 1. read t0_degc and t1_degc coefficients */
    uint8_t reg = HTS221_REGS_T0_DEGC_X8 | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    /* 2. read t1_t0_msb */
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_T1_T0_MSB, &tmp, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    /* 3. calc values */
    int16_t t0_degc_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | buf[0];
    int16_t t1_degc_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | buf[1];
    dev->t0_degc = t0_degc_x8_u16 >> 3;
    dev->t1_degc = t1_degc_x8_u16 >> 3;
    DEBUG("%s: t0_degc %" PRIi16 ", t1_degc %" PRIi16 "\n",
          DEBUG_FUNC, dev->t0_degc, dev->t1_degc);
    /* 4. read t0_out */
    reg = HTS221_REGS_T0_OUT_L | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    dev->t0_out = ((uint16_t)buf[1] << 8) | buf[0];
    /* 5. read t1_out */
    reg = HTS221_REGS_T1_OUT_L | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    dev->t1_out = ((uint16_t)buf[1] << 8) | buf[0];
    DEBUG("%s: t0_out %" PRIi16 ", t1_out %" PRIi16 "\n",
          DEBUG_FUNC, dev->t0_out, dev->t1_out);

    return HTS221_OK;
}

int hts221_init(hts221_t *dev, const hts221_params_t *params)
{
    uint8_t reg;

    memcpy(&dev->p, params, sizeof(hts221_params_t));

    i2c_acquire(BUS);
    /* try if we can interact with the device by reading its manufacturer ID */
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_WHO_AM_I, &reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs HTS221_REGS_WHO_AM_I failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }

    if (reg != HTS221_DEVICE_ID) {
        i2c_release(BUS);
        DEBUG("%s: invalid HTS221_DEVICE_ID!\n", DEBUG_FUNC);
        return -HTS221_NODEV;
    }
    i2c_release(BUS);

    /* reboot device before usage */
    if (hts221_reboot(dev) != HTS221_OK) {
        return -HTS221_NOBUS;
    }

    i2c_acquire(BUS);
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_AV_CONF, dev->p.avgx, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_write_regs HTS221_REGS_AV_CONF failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    reg = 0;
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_CTRL_REG1, reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);

    i2c_acquire(BUS);
    if ((_humidity_calibration(dev) != HTS221_OK) ||
        (_temperature_calibration(dev) != HTS221_OK)) {
        i2c_release(BUS);
        DEBUG("%s: read calibration parameters failed!\n", DEBUG_FUNC);
        return HTS221_ERROR;
    }
    i2c_release(BUS);
    DEBUG("%s: DONE!\n", DEBUG_FUNC);

    return HTS221_OK;
}

int hts221_one_shot(const hts221_t *dev)
{
    uint8_t reg = HTS221_REGS_CTRL_REG1_ODR_ONE_SHOT;

    /* first, disable any continuous measurement, enter one short mode */
    if (hts221_set_rate(dev, reg) != HTS221_OK) {
        return -HTS221_NOBUS;
    }

    i2c_acquire(BUS);
    /* second, read current settings */
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    /* third, enable one-shot */
    reg |= HTS221_REGS_CTRL_REG2_OS_EN;
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_CTRL_REG2, reg, 0) != 1) {
        i2c_release(BUS);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);

    return HTS221_OK;
}

int hts221_set_rate(const hts221_t *dev, const uint8_t rate)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_reg HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    reg |= rate;
    DEBUG("hts221_set_rate: %u\n", reg);
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_CTRL_REG1, reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);

    return HTS221_OK;
}

int hts221_reboot(const hts221_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    reg = HTS221_REGS_CTRL_REG2_BOOT;
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_CTRL_REG2, reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_write_reg HTS221_REGS_CTRL_REG2 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    /* loop until HTS221_REGS_CTRL_REG2_BOOT == 0 */
    do {
        i2c_read_reg(BUS, ADDR, HTS221_REGS_CTRL_REG2, &reg, 0);
    } while (reg & HTS221_REGS_CTRL_REG2_BOOT);
    i2c_release(BUS);

    return HTS221_OK;
}

static int _set_power(const hts221_t *dev, const bool active)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_CTRL_REG1, &reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_reg HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    if (active) {
        reg |= HTS221_REGS_CTRL_REG1_PD_ACTIVE;
    }
    else {
        reg &= ~HTS221_REGS_CTRL_REG1_PD_ACTIVE;
    }
    if (i2c_write_reg(BUS, ADDR, HTS221_REGS_CTRL_REG1, reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_write_reg HTS221_REGS_CTRL_REG1 failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);

    return HTS221_OK;
}

int hts221_power_on(const hts221_t *dev)
{
    return _set_power(dev, true);
}

int hts221_power_off(const hts221_t *dev)
{
    return _set_power(dev, false);
}

int hts221_get_state(const hts221_t *dev)
{
    uint8_t reg;

    i2c_acquire(BUS);
    if (i2c_read_reg(BUS, ADDR, HTS221_REGS_STATUS_REG, &reg, 0) != 1) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);

    return (int)reg;
}

int hts221_read_humidity(const hts221_t *dev, uint16_t *val)
{
    uint8_t buf[2];

    if (!(hts221_get_state(dev) & HTS221_REGS_STATUS_REG_HDA)) {
        DEBUG("%s: waiting for data ...\n", DEBUG_FUNC);
        while (!(hts221_get_state(dev) & HTS221_REGS_STATUS_REG_HDA)) {}
    }

    /* read raw humidity */
    i2c_acquire(BUS);
    uint8_t reg = HTS221_REGS_HUMIDITY_OUT_L | 0x80;

    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC,  buf[0], buf[1]);
    int16_t h_t_out = ((uint16_t)buf[1] << 8) | buf[0];
    DEBUG("%s, h_t_out: %" PRIi16 "\n", DEBUG_FUNC, h_t_out);

    /* compute RH [%] value by linear interpolation */
    int32_t tmp32 = (h_t_out - dev->h0_t0_out) * (dev->h1_rh - dev->h0_rh) * 10;
    DEBUG("%s, tmp32: %" PRIi32 "\n", DEBUG_FUNC, tmp32);
    *val = (tmp32 / (dev->h1_t0_out - dev->h0_t0_out)) + (dev->h0_rh * 10);
    /* cut of humidty at 100% */
    if (*val > 1000) {
        *val = 1000;
    }
    DEBUG("%s, val: %" PRIu16 "\n", DEBUG_FUNC, *val);

    return HTS221_OK;
}

int hts221_read_temperature(const hts221_t *dev, int16_t *val)
{
    uint8_t buf[2];

    if (!(hts221_get_state(dev) & HTS221_REGS_STATUS_REG_TDA)) {
        DEBUG("%s: waiting for data ...\n", DEBUG_FUNC);
        while (!(hts221_get_state(dev) & HTS221_REGS_STATUS_REG_TDA)) {}
    }

    /* read raw t_out */
    i2c_acquire(BUS);
    uint8_t reg = HTS221_REGS_TEMP_OUT_L | 0x80;
    if (i2c_read_regs(BUS, ADDR, reg, &buf[0], 2, 0) != 2) {
        i2c_release(BUS);
        DEBUG("%s: i2c_read_regs failed!\n", DEBUG_FUNC);
        return -HTS221_NOBUS;
    }
    i2c_release(BUS);
    DEBUG("%s: buf[0]=%u buf[1]=%u\n", DEBUG_FUNC, buf[0], buf[1]);
    int16_t t_out = ((uint16_t)buf[1] << 8) | buf[0];
    DEBUG("%s, t_out: %" PRIi16 "\n", DEBUG_FUNC, t_out);

    /* calculate actual temperature */
    int32_t tmp32 = (t_out - dev->t0_out) * (dev->t1_degc - dev->t0_degc) * 10;
    DEBUG("%s, tmp32: %" PRIi32 "\n", DEBUG_FUNC, tmp32);
    *val = (tmp32 / (dev->t1_out - dev->t0_out)) + (dev->t0_degc * 10);
    DEBUG("%s, val: %" PRIi16 "\n", DEBUG_FUNC, *val);

    return HTS221_OK;
}
