/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "sht4x.h"
#include "sht4x_params.h"

#define SHT4X_NUM ARRAY_SIZE(sht4x_params)
extern sht4x_dev_t sht4x_devs[SHT4X_NUM];

static bool _temp_valid[SHT4X_NUM] = { false };
static bool _hum_valid[SHT4X_NUM] = { false };
static int16_t _temp[SHT4X_NUM];
static int16_t _hum[SHT4X_NUM];

static unsigned _dev2index(const sht4x_dev_t *dev)
{
    for (unsigned i = 0; i < SHT4X_NUM; i++) {
        if (dev == &sht4x_devs[i]) {
            return i;
        }
    }
    return SHT4X_NUM;
}

static int _read(unsigned dev)
{
    int res;

    res = sht4x_read(&sht4x_devs[dev], &_temp[dev], &_hum[dev]);
    if (res != SHT4X_OK) {
        return res;
    }

    _temp_valid[dev] = true;
    _hum_valid[dev] = true;

    return SHT4X_OK;
}

static int read_temp(const void *dev, phydat_t *data)
{
    unsigned dev_index = _dev2index((const sht4x_dev_t *)dev);
    if (dev_index == SHT4X_NUM) {
        return -ECANCELED;
    }

    if (_temp_valid[dev_index] || _read(dev_index) == SHT4X_OK) {
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
    unsigned dev_index = _dev2index((const sht4x_dev_t *)dev);
    if (dev_index == SHT4X_NUM) {
        return -ECANCELED;
    }

    if (_hum_valid[dev_index] || _read(dev_index) == SHT4X_OK) {
        _hum_valid[dev_index] = false;
        data->val[0] = _hum[dev_index];
        data->unit = UNIT_PERCENT;
        data->scale = 0;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t sht4x_saul_driver_temperature = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t sht4x_saul_driver_humidity = {
    .read = read_hum,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM
};
