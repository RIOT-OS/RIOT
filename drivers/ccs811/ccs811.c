/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @{
 *
 * @file
 * @brief       Device driver implementation for the CCS811 gas sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "ccs811.h"
#include "ccs811_constants.h"
#include "ccs811_params.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define DEV_I2C             (dev->params.i2c_dev)
#define DEV_ADDR            (dev->params.i2c_addr)
#define DEV_WAKE_PIN        (dev->params.wake_pin)

static void _ccs811_poweron(const ccs811_t *dev)
{
    if (DEV_WAKE_PIN != GPIO_UNDEF) {
        DEBUG("[ccs811] power on device \n");
        gpio_clear(DEV_WAKE_PIN);
    }
}

static void _ccs811_poweroff(const ccs811_t *dev)
{
    if (DEV_WAKE_PIN != GPIO_UNDEF) {
        DEBUG("[ccs811] power off device \n");
        gpio_set(DEV_WAKE_PIN);
    }
}

int ccs811_init(ccs811_t *dev, const ccs811_params_t *params)
{
    memcpy(&dev->params, params, sizeof(ccs811_params_t));

    DEBUG("[ccs811] info: initializing CCS811 device %i at address %02X\n",
          DEV_I2C, DEV_ADDR);

    if (DEV_WAKE_PIN != GPIO_UNDEF) {
        gpio_init(DEV_WAKE_PIN, GPIO_OUT);
        gpio_clear(DEV_WAKE_PIN); /* power on the device */
    }

    /* Acquire I2C device */
    i2c_acquire(DEV_I2C);

    /* Read hardware ID */
    uint8_t hw_id;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, CCS811_REG_HW_ID, &hw_id, 0) < 0) {
        DEBUG("[ccs811] error: failed to read HW id register '%02X'\n",
              CCS811_REG_HW_ID);
        i2c_release(DEV_I2C);
        return -EIO;
    }

    DEBUG("[ccs811] info: hardware ID is '0x%02X'\n", hw_id);
    if (hw_id != CCS811_HW_ID) {
        DEBUG("[ccs811] error: wrong hardware ID, expected '0x%02X'\n",
              CCS811_HW_ID);
        i2c_release(DEV_I2C);
        return -ENODEV;
    }

    /* Check status */
    uint8_t status;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, CCS811_REG_STATUS, &status, 0) < 0) {
        DEBUG("[ccs811] error: failed to read status register '%02X'\n",
              CCS811_REG_STATUS);
        i2c_release(DEV_I2C);
        return -EIO;
    }

    /* Check for valid application */
    DEBUG("[ccs811] info: status is '0x%02X'\n", status);
    if (!(status & CCS811_STATUS_REG_APP_VALID)) {
        DEBUG("[ccs811] error: invalid application '%02X'\n", status);
        i2c_release(DEV_I2C);
        return -ECANCELED;
    }

    /* Start application */
    if (i2c_write_byte(DEV_I2C, DEV_ADDR, CCS811_REG_APP_START, 0) < 0) {
        DEBUG("[ccs811] error: cannot start application\n");
        i2c_release(DEV_I2C);
        return -ECANCELED;
    }

    /* Check application is in firmware mode status */
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, CCS811_REG_STATUS, &status, 0) < 0) {
        DEBUG("[ccs811] error: failed to read status register '%02X'\n",
              CCS811_REG_STATUS);
        i2c_release(DEV_I2C);
        return -EIO;
    }

    DEBUG("[ccs811] info: status is '0x%02X'\n", status);
    if (!(status & CCS811_STATUS_REG_FW_MODE)) {
        DEBUG("[ccs811] error: not in firmware mode '%02X'\n", status);
        i2c_release(DEV_I2C);
        return -ECANCELED;
    }

    if (ENABLE_DEBUG) {
        /* Read hardware version */
        uint8_t hw_version;
        if (i2c_read_reg(DEV_I2C, DEV_ADDR,
                         CCS811_REG_HW_VERSION, &hw_version, 0) < 0) {
            DEBUG("[ccs811] error: failed to read HW version register '%02X'\n",
                  CCS811_REG_HW_VERSION);
            i2c_release(DEV_I2C);
            return -EIO;
        }
        DEBUG("[ccs811] info: hardware version is '0x%02X'\n", hw_version);
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    _ccs811_poweroff(dev);

    return 0;
}

int ccs811_set_drive_mode(const ccs811_t *dev, ccs811_drive_mode_t mode)
{
    _ccs811_poweron(dev);

    /* Acquire I2C device */
    i2c_acquire(DEV_I2C);

    if (i2c_write_reg(DEV_I2C, DEV_ADDR,
                      CCS811_REG_MEAS_MODE, (uint8_t)(mode << 4), 0) < 0) {
        DEBUG("[ccs811] error: failed to set drive mode '%02X'\n",
              mode);
        i2c_release(DEV_I2C);
        return -EIO;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    _ccs811_poweroff(dev);

    return 0;
}

int ccs811_read_data(const ccs811_t *dev, uint16_t *eco2, uint16_t *tvoc)
{
    _ccs811_poweron(dev);

    /* Acquire I2C device */
    i2c_acquire(DEV_I2C);

    uint8_t status;
    if (i2c_read_reg(DEV_I2C, DEV_ADDR, CCS811_REG_STATUS, &status, 0) < 0) {
        DEBUG("[ccs811] error: failed to read status register '%02X'\n",
              CCS811_REG_STATUS);
        i2c_release(DEV_I2C);
        return -EIO;
    }

    DEBUG("[ccs811] info: status is '0x%02X'\n", status);
    if (!(status & CCS811_STATUS_REG_DATA_READY)) {
        DEBUG("[ccs811] error: data not ready '%02X'\n", status);
        i2c_release(DEV_I2C);
        return -ECANCELED;
    }

    uint8_t data[8];
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, CCS811_REG_ALG_RESULT_DATA, data, 8, 0) < 0) {
        DEBUG("[ccs811] error: failed to read result data register '%02X'\n",
              CCS811_REG_ALG_RESULT_DATA);
        /* Release I2C device */
        i2c_release(DEV_I2C);
        return -EIO;
    }

    DEBUG("[ccs811] read_data: ECO2: %i\n", (uint16_t) (data[0] << 8) + data[1]);
    DEBUG("[ccs811] read_data: TVOC: %i\n", (uint16_t) (data[2] << 8) + data[3]);

    if (eco2 != NULL) {
        *eco2 = (uint16_t)(data[0] <<8) + data[1];
    }

    if (tvoc != NULL) {
        *tvoc = (uint16_t) (data[2] <<8) + data[3];
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    _ccs811_poweroff(dev);

    return 0;
}

int ccs811_read_eco2(const ccs811_t *dev, uint16_t *eco2)
{
    return ccs811_read_data(dev, eco2, NULL);
}


int ccs811_read_tvoc(const ccs811_t *dev, uint16_t *tvoc)
{
    return ccs811_read_data(dev, NULL, tvoc);
}
