/*
 * Copyright (C) 2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_BME680
 * @{
 *
 * @file
 * @brief       Device driver implementation for the BME680
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 *
 * @}
 */

#include "bme680.h"
#include "bme680_params.h"
#include "bme680_internals.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define DEV_ADDR    (dev->params.i2c_addr)
#define DEV_I2C     (dev->params.i2c_dev)

int bme680_init(bme680_t *dev, const bme680_params_t *params)
{
    assert(dev && params);
    dev->params = *params;
    
    /*Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    uint8_t id = 0;
    printf("aquired\n");

    if (i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_RESET, BME680_RESET, 0) < 0){
        DEBUG("[bme680] error writing reset register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C;
    }

    if (i2c_read_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CHIP_ID, &id, 0) < 0){
        DEBUG("[bme680] error reading chip id register\n");
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return -BME680_ERR_I2C;
    }

    /* set oversampling mode do it in one write command */
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_HUM, params->temp_oversampling, 0); 
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_MEAS, params->temp_oversampling, 0); 
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_MEAS, params->pres_oversampling, 0);

    /* set gas sensor hot plate temperature set-point and heating duration */
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_GAS, params->gas_heating_time, 0);
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_GAS, params->gas_heating_temp, 0);
    
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_GAS, run_gas, 0);

    i2c_release(DEV_I2C);
    return 0;
}

int16_t bme680_read_tphg(const bme680_t *dev)
{
    /* set forced mode */
    if (dev)
    i2c_write_reg(DEV_I2C, DEV_ADDR, BME680_REGISTER_CTRL_MEAS, BME680_FORCED_MODE, 0);
    return 0;
}