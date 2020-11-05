/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme680
 * @brief       SAUL adaption for BME680 devices
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "bme680.h"
#include "bme680_params.h"
#include "xtimer.h"

extern bme680_t bme680_devs_saul[BME680_NUMOF];

/**
 * Temperature, pressure, humidity and gas sensor values are fetched by separate
 * saul functions. To avoid multiple waiting for the sensor, we read all sensor
 * values once, if necessary, and store them in local variables to provide them
 * in the separate saul read functions.
 */
static bool _temp_valid[BME680_NUMOF] = { false };
static bool _press_valid[BME680_NUMOF] = { false };
static bool _hum_valid[BME680_NUMOF] = { false };
static bool _gas_valid[BME680_NUMOF] = { false };
static int16_t _temp[BME680_NUMOF];
static int16_t _press[BME680_NUMOF];
static int16_t _hum[BME680_NUMOF];
static uint32_t _gas[BME680_NUMOF];

static unsigned _dev2index (const bme680_t *dev)
{
    /*
     * returns the index of the device in bme680_devs_saul[] or BME680_NUMOF
     * if not found
     */
    for (unsigned i = 0; i < BME680_NUMOF; i++) {
        if (dev == &bme680_devs_saul[i]) {
            return i;
        }
    }
    return BME680_NUMOF;
}

static int _read(int dev)
{
    /* measure and read sensor values */
    int res;
    if ((res = bme680_force_measurement(&bme680_devs_saul[dev])) != BME680_OK) {
        return res;
    }
    int drt;
    if ((drt = bme680_get_duration(&bme680_devs_saul[dev])) < 0) {
        return BME680_INVALID;
    }
    xtimer_msleep(drt);

    bme680_field_data_t data;
    if ((res = bme680_get_data(&bme680_devs_saul[dev], &data)) != BME680_OK) {
        return res;
    }

#if MODULE_BME680_FP
    _temp[dev] = data.temperature * 100;
    _press[dev] = data.pressure / 100;
    _hum[dev] = data.humidity * 100;
#else
    _temp[dev] = data.temperature;
    _press[dev] = data.pressure / 100;
    _hum[dev] = data.humidity / 10;
#endif
    _gas[dev] = (data.status & BME680_GASM_VALID_MSK) ? data.gas_resistance : 0;

    /* mark sensor values as valid */
    _temp_valid[dev] = true;
    _press_valid[dev] = true;
    _hum_valid[dev] = true;
    _gas_valid[dev] = true;
    return BME680_OK;
}

static int read_temp(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const bme680_t *)dev);
    if (dev_index == BME680_NUMOF) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_temp_valid[dev_index] || _read(dev_index) == BME680_OK) {
        /* mark local variable as invalid */
        _temp_valid[dev_index] = false;

        data->val[0] = _temp[dev_index];
        data->unit = UNIT_TEMP_C;
        data->scale = -2;
        return 1;
    }
    return -ECANCELED;
}

static int read_press(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const bme680_t *)dev);
    if (dev_index == BME680_NUMOF) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_press_valid[dev_index] || _read(dev_index) == BME680_OK) {
        /* mark local variable as invalid */
        _press_valid[dev_index] = false;

        data->val[0] = _press[dev_index];
        data->unit = UNIT_PA;
        data->scale = 2;
        return 1;
    }
    return -ECANCELED;
}

static int read_hum(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const bme680_t *)dev);
    if (dev_index == BME680_NUMOF) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_hum_valid[dev_index] || _read(dev_index) == BME680_OK) {
        /* mark local variable as invalid */
        _hum_valid[dev_index] = false;

        data->val[0] = _hum[dev_index];
        data->unit = UNIT_PERCENT;
        data->scale = -2;
        return 1;
    }
    return -ECANCELED;
}

static int read_gas(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const bme680_t *)dev);
    if (dev_index == BME680_NUMOF) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_gas_valid[dev_index] || _read(dev_index) == BME680_OK) {
        /* mark local variable as invalid */
        _gas_valid[dev_index] = false;

        if (_gas[dev_index] > INT16_MAX) {
            data->val[0] = _gas[dev_index] / 1000;
            data->scale = 3;
        }
        else {
            data->val[0] = _gas[dev_index];
            data->scale = 0;
        }
        data->unit = UNIT_OHM;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t bme680_saul_driver_temperature = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t bme680_saul_driver_pressure = {
    .read = read_press,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS
};

const saul_driver_t bme680_saul_driver_humidity = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM
};

const saul_driver_t bme680_saul_driver_gas = {
    .read = read_gas,
    .write = saul_notsup,
    .type = SAUL_SENSE_GAS
};
