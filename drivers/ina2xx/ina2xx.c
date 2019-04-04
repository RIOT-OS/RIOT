/*
 * Copyright (C) 2015 Eistec AB
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ina2xx
 * @{
 *
 * @file
 * @brief       Device driver implementation for Texas Instruments INA2XX High
 *              or Low Side, Bi-Directional CURRENT/POWER MONITOR with Two-Wire
 *              Interface
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>

#include "ina2xx.h"
#include "ina2xx_defines.h"
#include "periph/i2c.h"
#include "byteorder.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/*
 * The value in the current register is obtained by:
 *
 *     I = (V_shunt * C) / 4096
 *
 * Where V_shunt is the value in the shunt voltage register and C is the
 * value programmed (upon driver initialization) into the calibration register
 */
#define CURRENT_QUOTIENT    (4096LU)

/** @brief Read one 16 bit register from a INA2XX device and swap byte order, if necessary. */
static int ina2xx_read_reg(const ina2xx_t *dev, uint8_t reg, uint16_t *out)
{
    union {
        uint8_t c[2];
        uint16_t u16;
    } tmp;
    int status = 0;

    status = i2c_read_regs(dev->params.i2c, dev->params.addr, reg, &tmp.c[0],
                           2, 0);

    if (status < 0) {
        return status;
    }

    *out = ntohs(tmp.u16);
    return 0;
}

/** @brief Write one 16 bit register to a INA2XX device and swap byte order, if necessary. */
static int ina2xx_write_reg(const ina2xx_t *dev, uint8_t reg, uint16_t in)
{
    union {
        uint8_t c[2];
        uint16_t u16;
    } tmp;
    int status = 0;

    tmp.u16 = htons(in);

    status = i2c_write_regs(dev->params.i2c, dev->params.addr, reg, &tmp.c[0],
                            2, 0);

    if (status < 0) {
        return status;
    }

    return 0;
}


int ina2xx_init(ina2xx_t *dev, const ina2xx_params_t *params)
{
    uint16_t config;
    int status;
    if (!dev || !params) {
        return -EINVAL;
    }

    dev->params = *params;
    /* Reset device */
    status = ina2xx_write_reg(dev, INA2XX_REG_CONFIGURATION, INA2XX_RESET);
    if (status < 0) {
        return status;
    }

    /* Check if default config is preset after reset */
    status = ina2xx_read_reg(dev, INA2XX_REG_CONFIGURATION, &config);
    if (status < 0) {
        return status;
    }

    if (config != INA2XX_DEFCONFIG) {
        DEBUG("[ina2xx]: Reset did't restore default config. Wrong device?\n");
        return -ENODEV;
    }

    status = ina2xx_write_reg(dev, INA2XX_REG_CONFIGURATION, params->config);
    if (status < 0) {
        return status;
    }

    /* Set calibration register based on the shunt resistor.
     * ==> Current will be in mA
     * Multiply by 100
     * ==> Current will be in mA
     */
    uint32_t calib = (100 * CURRENT_QUOTIENT) / params->rshunt_mohm;
    /* Divide by 2^i_range to reduce the resolution by factor 2^i_range */
    calib >>= params->i_range;

    if (calib > UINT16_MAX) {
        return -ERANGE;
    }

    return ina2xx_write_reg(dev, INA2XX_REG_CALIBRATION, (uint16_t)calib);
}

int ina2xx_read_shunt(const ina2xx_t *dev, int16_t *voltage)
{
    return ina2xx_read_reg(dev, INA2XX_REG_SHUNT_VOLTAGE, (uint16_t *)voltage);
}

int ina2xx_read_bus(const ina2xx_t *dev, uint16_t *voltage)
{
    uint16_t tmp;
    int status = ina2xx_read_reg(dev, INA2XX_REG_BUS_VOLTAGE, &tmp);
    if (status < 0) {
        return status;
    }

    /* The voltage given by bits 15-3 in steps of 4 mV
     * ==> Take bits 15-3, shift 3 bits right, multiply by 4
     * ==> Same as: Take bits 15-3, shift 3 bits right, shift 2 bits left
     * ==> Same as: Take bits 15-3, shift 1 bit right
     */
    *voltage = (tmp & ~0x7) >> 1;

    if (tmp & INA2XX_VBUS_OVF) {
        return -EDOM;
    }

    return (tmp & INA2XX_VBUS_CNVR) ? 1 : 0;
}

int ina2xx_read_current(const ina2xx_t *dev, int32_t *current)
{
    int16_t tmp;
    int status = ina2xx_read_reg(dev, INA2XX_REG_CURRENT, (uint16_t *)&tmp);
    if (status < 0) {
        return status;
    }

    /*
     * The calibration is chosen according to the selected value in
     * dev->params.i_range, so that tmp * 2^i_range gives us the
     * current in E-05 A. We can thus simple use a left shift to convert
     * the current into E-05 A.
     */
    *current = ((int32_t)tmp) << dev->params.i_range;
    return 0;
}

int ina2xx_read_power(const ina2xx_t *dev, uint32_t *power)
{
    int status;
    uint16_t tmp;
    status = ina2xx_read_reg(dev, INA2XX_REG_POWER, &tmp);
    if (status < 0) {
        return status;
    }

    /*
     * The resolution of the raw power value depends only on the resolution
     * of the raw current value, as the bus voltage has a resolution of 4 mV
     * regardless of configuration and calibration values. The product of
     * bus voltage and raw current value is divided by 5000, this results in
     * the following resolutions:
     *
     * Res current | res power
     * 0.01 mA     | 0.2 mW
     * 0.02 mA     | 0.4 mW
     * 0.04 mA     | 0.8 mW
     * ...
     *
     * ==> multiply by 2^(1 + i_range) to get power in E-04W
     */
    *power = ((uint32_t)tmp) << (1 + dev->params.i_range);
    return 0;
}
