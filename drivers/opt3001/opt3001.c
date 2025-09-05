/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_opt3001
 * @{
 *
 * @file
 * @brief       Driver for the TI OPT3001 Ambient Light Sensor.
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>

#include "opt3001.h"
#include "opt3001_regs.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "log.h"
#include "byteorder.h"
#include "math.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C     (dev->params.i2c_dev) /**< BUS */
#define DEV_ADDR    (dev->params.i2c_addr) /**< ADDR */

int opt3001_init(opt3001_t *dev, const opt3001_params_t *params)
{
    /* Check parameters */
    assert(dev && params);

    uint16_t reg;

    dev->params = *params;

    /* Test device id */
    i2c_acquire(DEV_I2C);
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_DEVICE_ID, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error reading device ID!\n");
        return -OPT3001_ERROR_BUS;
    }

    reg = htons(reg);
    if (reg != OPT3001_DID_VALUE) {
        return -OPT3001_ERROR_DEV;
    }

    /* Set range number, mode of conversion and conversion time */
    reg = OPT3001_CONFIG_RN_FSR;
    reg |= OPT3001_REGS_CONFIG_MOC(OPT3001_CONFIG_M_SHUTDOWN);
    reg |= OPT3001_REGS_CONFIG_CT(CONFIG_OPT3001_CONVERSION_TIME);

    /* Configure for latched window-style comparison operation */
    reg |= OPT3001_REGS_CONFIG_L;
    reg &= ~OPT3001_REGS_CONFIG_POL;
    reg &= ~OPT3001_REGS_CONFIG_ME;
    reg &= ~OPT3001_REGS_CONFIG_FC_MASK;

    reg = htons(reg);

    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_init: Error setting device configuration\n");
        return -OPT3001_ERROR_BUS;
    }
    i2c_release(DEV_I2C);

    return OPT3001_OK;
}

int opt3001_reset(const opt3001_t *dev)
{
    uint16_t reg = OPT3001_CONFIG_RESET;
    reg &= (0xF0FF | OPT3001_REGS_CONFIG_CT(CONFIG_OPT3001_CONVERSION_TIME));
    reg = htons(reg);

    /* Acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);
    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_reset: Error setting device configuration\n");
        return -OPT3001_ERROR_BUS;
    }
    i2c_release(DEV_I2C);
    return OPT3001_OK;
}

int opt3001_set_active(const opt3001_t *dev)
{
    uint16_t reg;

    i2c_acquire(DEV_I2C);

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_set_active: Error reading BUS!\n");
        return -OPT3001_ERROR_BUS;
    }

    reg = htons(reg);
    reg |= OPT3001_REGS_CONFIG_MOC(OPT3001_CONFIG_M_CONTINUOUS);

    reg = htons(reg);
    if (i2c_write_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_set_active: Error setting device configuration\n");
        return -OPT3001_ERROR_BUS;
    }

    i2c_release(DEV_I2C);
    return OPT3001_OK;
}

int opt3001_read_lux(const opt3001_t *dev, uint32_t *convl)
{
    uint8_t exponent;
    uint16_t reg, mantissa;
    uint32_t conversion_time = 0;

    i2c_acquire(DEV_I2C);

    while (1) {
        if (conversion_time >= OPT3001_CONVERSION_TIME_COMBINED) {
            i2c_release(DEV_I2C);
            LOG_ERROR("opt3001_read: Conversion in progress!\n");
            return -OPT3001_ERROR;
        }

        if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_CONFIG, &reg, 2, 0) < 0) {
            i2c_release(DEV_I2C);
            LOG_ERROR("opt3001_read: Error reading BUS!\n");
            return -OPT3001_ERROR_BUS;
        }

        if (htons(reg) & OPT3001_REGS_CONFIG_CRF) {
            break;
        }

        i2c_release(DEV_I2C);
        xtimer_usleep(1000);
        i2c_acquire(DEV_I2C);

        conversion_time += 1000;
    }

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, OPT3001_REGS_RESULT, &reg, 2, 0) < 0) {
        i2c_release(DEV_I2C);
        LOG_ERROR("opt3001_read: Error reading BUS!\n");
        return -OPT3001_ERROR_BUS;
    }

    exponent = OPT3001_REGS_REG_EXPONENT(htons(reg));
    mantissa = OPT3001_REGS_REG_MANTISSA(htons(reg));

    *convl = (1 << exponent) * mantissa * 10;

    i2c_release(DEV_I2C);
    return OPT3001_OK;
}
