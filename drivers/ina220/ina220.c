/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_ina220
 * @{
 *
 * @file
 * @brief       Device driver implementation for Texas Instruments INA220 High
 *              or Low Side, Bi-Directional CURRENT/POWER MONITOR with Two-Wire
 *              Interface
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stdint.h>

#include "ina220.h"
#include "ina220-regs.h"
#include "periph/i2c.h"
#include "byteorder.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/** @brief Read one 16 bit register from a INA220 device and swap byte order, if necessary. */
static int ina220_read_reg(ina220_t *dev, uint8_t reg, uint16_t *out)
{
    union {
        char c[2];
        uint16_t u16;
    } tmp = { .u16 = 0 };
    int status = 0;

    status = i2c_read_regs(dev->i2c, dev->addr, reg, &tmp.c[0], 2);

    if (status != 2) {
        return -1;
    }

    *out = NTOHS(tmp.u16);
    return 0;
}

/** @brief Write one 16 bit register to a INA220 device and swap byte order, if necessary. */
static int ina220_write_reg(ina220_t *dev, uint8_t reg, uint16_t in)
{
    union {
        char c[2];
        uint16_t u16;
    } tmp = { .u16 = 0 };
    int status = 0;

    tmp.u16 = HTONS(in);

    status = i2c_write_regs(dev->i2c, dev->addr, reg, &tmp.c[0], 2);

    if (status != 2) {
        return -1;
    }

    return 0;
}


int ina220_init(ina220_t *dev, i2c_t i2c, uint8_t address)
{
    /* write device descriptor */
    dev->i2c = i2c;
    dev->addr = address;
    return 0;
}

int ina220_set_calibration(ina220_t *dev, uint16_t calibration)
{
    return ina220_write_reg(dev, INA220_REG_CALIBRATION, calibration);
}

int ina220_set_config(ina220_t *dev, uint16_t config)
{
    return ina220_write_reg(dev, INA220_REG_CONFIGURATION, config);
}

int ina220_read_shunt(ina220_t *dev, int16_t *voltage)
{
    return ina220_read_reg(dev, INA220_REG_SHUNT_VOLTAGE, (uint16_t *)voltage);
}

int ina220_read_bus(ina220_t *dev, int16_t *voltage)
{
    return ina220_read_reg(dev, INA220_REG_BUS_VOLTAGE, (uint16_t *)voltage);
}

int ina220_read_current(ina220_t *dev, int16_t *current)
{
    return ina220_read_reg(dev, INA220_REG_CURRENT, (uint16_t *)current);
}

int ina220_read_power(ina220_t *dev, int16_t *power)
{
    return ina220_read_reg(dev, INA220_REG_POWER, (uint16_t *)power);
}
