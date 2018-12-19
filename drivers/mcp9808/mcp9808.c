/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp9808
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MCP9808 temperature sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <math.h>

#include "log.h"
#include "mcp9808.h"
#include "mcp9808_constants.h"
#include "mcp9808_params.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c_dev)
#define DEV_ADDR     (dev->params.i2c_addr)

int _read16(const mcp9808_t *dev, uint8_t pointer, uint16_t *reg)
{
    uint8_t buf[2];
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, pointer, &buf, 2, 0) != 0) {
        DEBUG("[mcp9808] Error: cannot read register 0x%02X\n", pointer);
        i2c_release(DEV_I2C);
        return -MCP9808_ERR_I2C;
    }

    *reg = (buf[0] << 8) | buf[1];

    return MCP9808_OK;
}

int _write16(const mcp9808_t *dev, uint8_t pointer, uint16_t *reg)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)((*reg) >> 8);
    buf[1] = (uint8_t)(*reg);
    if (i2c_write_regs(DEV_I2C, DEV_ADDR, pointer, &buf, 2, 0) != 0) {
        DEBUG("[mcp9808] Error: cannot write register 0x%02X\n", pointer);
        i2c_release(DEV_I2C);
        return -MCP9808_ERR_I2C;
    }

    return MCP9808_OK;
}

int mcp9808_init(mcp9808_t *dev, const mcp9808_params_t *params)
{
    dev->params = *params;

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Check device ID */
    uint8_t checkid;
    i2c_read_reg(DEV_I2C, DEV_ADDR, MCP9808_REG_DEVICE_ID_REV, &checkid, 0);
    if (checkid != MCP9808_DEVICE_ID) {
        DEBUG("[mcp9808] Error: wrong device ID 0x%02X\n", checkid);
        i2c_release(DEV_I2C);
        return -MCP9808_ERR_NODEV;
    }

    /* Reset config register to default values */
    uint16_t init = 0;
    if (_write16(dev, MCP9808_REG_CONFIG, &init) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot write config register\n");
        i2c_release(DEV_I2C);
        return -MCP9808_ERR_I2C;
    }

    /* Write measure resolution */
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, MCP9808_REG_RESOLUTION,
                      dev->params.resolution, 0) != 0) {
        DEBUG("[mcp9808] Error: cannot read resolution register\n");
        i2c_release(DEV_I2C);
        return -MCP9808_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    DEBUG("[mcp9808] Initialization succeeded\n");

    return MCP9808_OK;
}

int mcp9808_read_temperature(const mcp9808_t *dev, int16_t *temperature)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint16_t ta;
    if (_read16(dev, MCP9808_REG_T_AMBIENT, &ta) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot read ambient temperature register\n");
        return -MCP9808_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    /* convert temperature to d°C */
    uint8_t ub = (uint8_t)(ta >> 8) & 0x1f; /* clear flag bits */
    uint8_t lb = (uint8_t)ta;
    DEBUG("[mcp9808] UB: 0x%02X, LB:0x%02X\n", ub, lb);
    if ((ub & 0x10) == 0x10) { /* TA < 0°C */
        ub = ub & 0x0f; /* Clear SIGN */
        *temperature = 256 - ((ub * 160) + ((lb * 10) / 16));
    } else { /* TA >= 0°C */
        *temperature = ((ub * 160) + ((lb * 10) / 16));
    }

    DEBUG("[mcp9808] Computed T: %i\n", *temperature);

    return MCP9808_OK;
}

int mcp9808_wakeup(const mcp9808_t *dev)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint16_t config;
    if (_read16(dev, MCP9808_REG_CONFIG, &config) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot read config register\n");
        return -MCP9808_ERR_I2C;
    }

    config &= ~MCP9808_REG_CONFIG_SHDN;
    DEBUG("[mcp9808] wake_up: setting config to 0x%04X\n", config);
    if (_write16(dev, MCP9808_REG_CONFIG, &config) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot write config register\n");
        return -MCP9808_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return MCP9808_OK;
}

int mcp9808_shutdown(const mcp9808_t *dev)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint16_t config;
    if (_read16(dev, MCP9808_REG_CONFIG, &config) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot read config register\n");
        return -MCP9808_ERR_I2C;
    }

    config |= MCP9808_REG_CONFIG_SHDN;
    DEBUG("[mcp9808] shutdown: setting config to 0x%04X\n", config);
    if (_write16(dev, MCP9808_REG_CONFIG, &config) != MCP9808_OK) {
        DEBUG("[mcp9808] Error: cannot write config register\n");
        return -MCP9808_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return MCP9808_OK;
}
