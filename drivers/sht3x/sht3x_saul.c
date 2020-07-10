/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht3x
 * @brief       SAUL adaption for Sensirion SHT30/SHT31/SHT35 devices
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "sht3x.h"
#include "sht3x_params.h"

#define SHT3X_NUM      ARRAY_SIZE(sht3x_params)
extern sht3x_dev_t sht3x_devs[SHT3X_NUM];

/**
 * Humidity and temperature sensor values are fetched by separate saul
 * functions. To avoid double waiting for the sensor and readout of the
 * sensor values, we read both sensor values once, if necessary, and
 * store them in local variables to provide them in the separate saul
 * read functions.
 */
static bool _temp_valid[SHT3X_NUM] = { false };
static bool _hum_valid[SHT3X_NUM] = { false };
static int16_t _temp[SHT3X_NUM];
static int16_t _hum[SHT3X_NUM];

static unsigned _dev2index (const sht3x_dev_t *dev)
{
    /*
     * returns the index of the device in sht3x_devs[] or SHT3_NUM
     * if not found
     */
    for (unsigned i = 0; i < SHT3X_NUM; i++) {
        if (dev == &sht3x_devs[i]) {
            return i;
        }
    }
    return SHT3X_NUM;
}

static int _read(int dev)
{
    /* read both sensor values */
    unsigned res = sht3x_read(&sht3x_devs[dev], &_temp[dev], &_hum[dev]);
    if (res != SHT3X_OK) {
        return res;
    }
    /* mark both sensor values as valid */
    _temp_valid[dev] = true;
    _hum_valid[dev] = true;
    return SHT3X_OK;
}

static int read_temp(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const sht3x_dev_t *)dev);
    if (dev_index == SHT3X_NUM) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_temp_valid[dev_index] || _read(dev_index) == SHT3X_OK) {
        /* mark local variable as invalid */
        _temp_valid[dev_index] = false;

        data->val[0] = _temp[dev_index];
        data->unit = UNIT_TEMP_C;
        data->scale = -2;
        return 1;
    }
    return -ECANCELED;
}

static int read_hum(const void *dev, phydat_t *data)
{
    /* find the device index */
    unsigned dev_index = _dev2index((const sht3x_dev_t *)dev);
    if (dev_index == SHT3X_NUM) {
        /* return with error if device index could not be found */
        return -ECANCELED;
    }

    /* either local variable is valid or fetching it was successful */
    if (_hum_valid[dev_index] || _read(dev_index) == SHT3X_OK) {
        /* mark local variable as invalid */
        _hum_valid[dev_index] = false;

        data->val[0] = _hum[dev_index];
        data->unit = UNIT_PERCENT;
        data->scale = -2;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t sht3x_saul_driver_temperature = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t sht3x_saul_driver_humidity = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM
};
