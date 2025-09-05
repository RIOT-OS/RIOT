/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ds75lx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the DS75LX temperature sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#include "ds75lx.h"
#include "ds75lx_internals.h"
#include "ds75lx_params.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C      (dev->params.i2c)
#define DEV_ADDR     (dev->params.addr)

static int _update_configuration_bits(const ds75lx_t *dev, uint8_t bit,
                                      uint8_t mask, bool set)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t config;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, DS75LX_REG_CONFIGURATION, &config, 0) < 0) {
        DEBUG("[ds75lx] error reading configuration register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    DEBUG("[ds75lx] initial configuration register value: 0x%02X\n", config);

    /* clear bits */
    config &= ~mask;

    /* set bits if required */
    if (set) {
        config |= bit;
    }

    DEBUG("[ds75lx] configuration register value: 0x%02X\n", config);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, DS75LX_REG_CONFIGURATION, config, 0) < 0) {
        DEBUG("[ds75lx] error writing configuration register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return DS75LX_OK;
}

int ds75lx_init(ds75lx_t *dev, const ds75lx_params_t *params)
{
    dev->params = *params;

    /* Set resolution bits + force shutdown of sensor */
    return _update_configuration_bits(dev,
        (dev->params.resolution << DS75LX_CONF_R0_POS) | (1 << DS75LX_CONF_SD_POS),
        (DS75LX_CONF_R0_MASK | (1 << DS75LX_CONF_SD_POS)), true);
}

int ds75lx_read_temperature(const ds75lx_t *dev, int16_t *temperature)
{
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t tmp[2];
    uint16_t temp;

    if (i2c_read_regs(DEV_I2C, DEV_ADDR, DS75LX_REG_TEMPERATURE, tmp, 2, 0) < 0) {
        DEBUG("[ds75lx] error reading temperature register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -DS75LX_ERR_I2C;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    temp = (tmp[0] << 8) | tmp[1];
    DEBUG("[ds75lx] temperature register content 0x%04X\n", temp);

    /* isolate integer part of the temperature */
    int8_t temp_int = (temp & 0xff00) >> 8;
    /* compute fractional part of the temperature, the LSB bits 3 to 0 are
    always zero and not used in the conversion */
    uint8_t temp_frac = (temp & 0x00f0) >> 4;

    /* fractional part is a multiple of 0.0625. Temperature is returned in c°C */
    *temperature  = (temp_int * 100 + ((uint16_t)temp_frac * 100 >> 4));

    return DS75LX_OK;
}

int ds75lx_wakeup(const ds75lx_t *dev)
{
    /* disable shutdown bit in configuration register */
    int ret = _update_configuration_bits(dev, (1 << DS75LX_CONF_SD_POS),
                                         (1 << DS75LX_CONF_SD_POS), false);

    if (ret == DS75LX_OK) {
        /* Wait max conversion time (depends on resolution) */
        xtimer_msleep((DS75LX_MAX_CONVERSION_TIME << dev->params.resolution));
    }

    return ret;
}

int ds75lx_shutdown(const ds75lx_t *dev)
{
    /* enable shutdown bit in configuration register */
    return _update_configuration_bits(dev, (1 << DS75LX_CONF_SD_POS),
                                      (1 << DS75LX_CONF_SD_POS), true);
}
