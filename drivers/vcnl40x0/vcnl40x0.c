/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @{
 *
 * @file
 * @brief       Device driver implementation for VCNL40X0 Proximity and Ambient Light devices.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "vcnl40x0.h"
#include "vcnl40x0_internals.h"
#include "vcnl40x0_params.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define DEV_I2C             (dev->params.i2c_dev)
#define DEV_ADDR            (dev->params.i2c_addr)

/* Internal functions */
static int _set_command_reg(const vcnl40x0_t *dev, uint8_t reg)
{
    if (i2c_write_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_COMMAND, reg, 0) != 0) {
        return -1;
    }

    return 0;
}

static int _get_command_reg(const vcnl40x0_t *dev, uint8_t *reg)
{
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_COMMAND, reg, 0) != 0) {
        return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
 *                          VCNL40X0 Core API                                *
 *---------------------------------------------------------------------------*/

int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params)
{
    dev->params = *params;

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Check sensor ID */
    uint8_t checkid;
    i2c_read_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_PRODUCT_ID,
                  &checkid, 0);
    if ((checkid & VCNL40X0_PRODUCT_MASK_PRODUCT_ID) != VCNL40X0_PRODUCT_ID) {
        DEBUG("[vcnl40x0] Error: wrong device ID '0x%02x', expected '0x%02x'\n",
              checkid, VCNL40X0_PRODUCT_ID);
        i2c_release(DEV_I2C);
        return -VCNL40X0_ERR_NODEV;
    }

    /* LED current cannot be above 20 */
    if (dev->params.led_current > 20) {
        dev->params.led_current = 20;
    }

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_PROXIMITY_CURRENT,
                                        dev->params.led_current, 0) != 0) {
        DEBUG("[vcnl40x0] Error: failed to set proximity current\n");
        i2c_release(DEV_I2C);
        return -VCNL40X0_ERR_I2C;
    }

    if (_set_command_reg(dev, VCNL40X0_COMMAND_ALL_DISABLE) < 0) {
        DEBUG("[vcnl40x0] Error: failed to set disable all commands\n");
        i2c_release(DEV_I2C);
        return -VCNL40X0_ERR_I2C;
    }

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_PROXIMITY_RATE,
                      dev->params.proximity_rate, 0) != 0) {
        DEBUG("[vcnl40x0] Error: failed to set proximity rate\n");
        i2c_release(DEV_I2C);
        return -VCNL40X0_ERR_I2C;
    }

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, VCNL40X0_REG_AMBIENT_PARAMETER,
                      dev->params.ambient_rate |
                      VCNL40X0_AMBIENT_PARA_AUTO_OFFSET_ENABLE |
                      dev->params.ambient_avg, 0) != 0) {
        DEBUG("[vcnl40x0] Error: failed to set ambient light rate\n");
        i2c_release(DEV_I2C);
        return -VCNL40X0_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    DEBUG("[vcnl40x0] info: vcnl40x0 sensor initialized with success\n");

    return VCNL40X0_OK;
}

uint16_t vcnl40x0_read_proximity(const vcnl40x0_t *dev)
{
    i2c_acquire(DEV_I2C);

    _set_command_reg(dev,
                     VCNL40X0_COMMAND_PROX_ENABLE |
                     VCNL40X0_COMMAND_PROX_ON_DEMAND);
    uint16_t cnt = 0xffff;
    while (cnt--) {
        uint8_t reg;
        _get_command_reg(dev, &reg);
        if (reg & VCNL40X0_COMMAND_MASK_PROX_DATA_READY) {
            uint8_t prox_buf[2];
            i2c_read_regs(DEV_I2C, DEV_ADDR, VCNL40X0_REG_PROXIMITY_VALUE,
                          &prox_buf, 2, 0);
            uint16_t res = (uint16_t)((prox_buf[0] << 8) | prox_buf[1]);
            DEBUG("[vcnl40x0] Proximity read: %i\n", res);
            i2c_release(DEV_I2C);
            return res;
        }
    }

    i2c_release(DEV_I2C);
    return 0;
}

uint16_t vcnl40x0_read_ambient_light(const vcnl40x0_t *dev)
{
    i2c_acquire(DEV_I2C);

    _set_command_reg(dev,
                     VCNL40X0_COMMAND_AMBI_ENABLE |
                     VCNL40X0_COMMAND_AMBI_ON_DEMAND);

    uint16_t cnt = 0xffff;
    while (cnt--) {
        uint8_t reg;
        _get_command_reg(dev, &reg);
        if (reg & VCNL40X0_COMMAND_MASK_AMBI_DATA_READY) {
            uint8_t ambient_buf[2];
            i2c_read_regs(DEV_I2C, DEV_ADDR, VCNL40X0_REG_AMBIENT_VALUE,
                          &ambient_buf, 2, 0);
            uint16_t res = (uint16_t)((ambient_buf[0] << 8) | ambient_buf[1]);
            DEBUG("[vcnl40x0] Ambient light read: %i\n", res);
            i2c_release(DEV_I2C);
            return res;
        }
    }

    i2c_release(DEV_I2C);

    return 0;
}

uint16_t vcnl40x0_read_illuminance(const vcnl40x0_t *dev)
{
    return vcnl40x0_read_ambient_light(dev) >> 2;
}
