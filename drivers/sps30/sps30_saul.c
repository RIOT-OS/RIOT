/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sps30
 * @brief       SAUL adaption for Sensirion SPS30 sensor
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @file
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "sps30.h"
#include "sps30_params.h"

/**
 * @brief   Mapping of sensor values to logical SAUL instances
 */
#define SPS30_SAUL_VAL_IDX_MC_PM_1_2P5_4      (0)
#define SPS30_SAUL_VAL_IDX_MC_PM_10           (1)
#define SPS30_SAUL_VAL_IDX_NC_PM_0P5_1_2P5    (2)
#define SPS30_SAUL_VAL_IDX_NC_PM_4_10         (3)
#define SPS30_SAUL_VAL_IDX_PS                 (4)

/**
 * @brief   Number of logical saul devices per physical sensor
 */
#define SPS30_SAUL_DEV_NUM                    (SPS30_SAUL_VAL_IDX_PS + 1)

extern sps30_t sps30_devs[SPS30_NUM];

/* contains a temporary copy of all sensor readings to allow returning all
 * values as one consistent group over multiple read calls */
static sps30_data_t _readings[SPS30_NUM];
static bool _valid[SPS30_NUM][SPS30_SAUL_DEV_NUM] = { {false} };

static unsigned _dev2index (const sps30_t *dev)
{
    for (unsigned i = 0; i < SPS30_NUM; i++) {
        if (dev == &sps30_devs[i]) {
            return i;
        }
    }
    return SPS30_NUM;
}

static void _float_fit(float *src, phydat_t *data, size_t dim, uint32_t mul)
{
    int32_t i32[dim];

    for (unsigned i = 0; i < dim; i++) {
        i32[i] = src[i] * mul;
    }

    data->scale = 0;
    phydat_fit(data, &i32[0], dim);
}

static int _read(const void *dev, phydat_t *data, unsigned int val_idx)
{
    /* find the device index */
    unsigned dev_idx = _dev2index((sps30_t*)dev);

    if (dev_idx == SPS30_NUM) {
        return -ECANCELED;
    }

    /* if data isn't valid form the last reading anymore, read again */
    if (!_valid[dev_idx][val_idx]) {
        int error_state = SPS30_OK;
        while (!sps30_data_ready(&sps30_devs[dev_idx], &error_state)) {
            if (error_state != SPS30_OK) {
                return -ECANCELED;
            }
        }
        if (!(sps30_read_measurement(&sps30_devs[dev_idx],
                                     &_readings[dev_idx]) == SPS30_OK)) {
            /* failure on read may corrupt _readings -> mark invalid */
            for (unsigned i = 0; i < SPS30_SAUL_DEV_NUM; i++) {
                _valid[dev_idx][i] = false;
            }
            return -ECANCELED;
        }
        for (unsigned i = 0; i < SPS30_SAUL_DEV_NUM; i++) {
            _valid[dev_idx][i] = true;
        }
    }

    /* mark read values as invalid */
    _valid[dev_idx][val_idx] = false;

    switch (val_idx) {
        case SPS30_SAUL_VAL_IDX_MC_PM_1_2P5_4:
            _float_fit(&_readings[dev_idx].mc_pm1, data, 3, 1000);
            data->unit = UNIT_GPM3;
            data->scale -= 9; /* fitted [ng/m^3] but unit is [g/m^3] */
            return 3;
        case SPS30_SAUL_VAL_IDX_MC_PM_10:
            _float_fit(&_readings[dev_idx].mc_pm10, data, 1, 1000);
            data->unit = UNIT_GPM3;
            data->scale = -9; /* fitted [ng/m^3] but unit is [g/m^3] */
            return 1;
        case SPS30_SAUL_VAL_IDX_NC_PM_0P5_1_2P5:
            _float_fit(&_readings[dev_idx].nc_pm0_5, data, 3, 1000);
            data->unit = UNIT_CPM3;
            data->scale = 3; /* fitted [#/dm^3] but unit is [#/m^3] */
            return 3;
        case SPS30_SAUL_VAL_IDX_NC_PM_4_10:
            _float_fit(&_readings[dev_idx].nc_pm4, data, 2, 1000);
            data->unit = UNIT_CPM3;
            data->scale = 3; /* fitted [#/dm^3] but unit is [#/m^3] */
            return 2;
        case SPS30_SAUL_VAL_IDX_PS:
            _float_fit(&_readings[dev_idx].ps, data, 1, 1000);
            data->unit = UNIT_M;
            data->scale -= 9; /* fitted [nm] but unit is [m] */
            return 1;
    }

    return -ECANCELED;
}

static int read_mc_pm_1_2p5_4(const void *dev, phydat_t *data)
{
    return _read(dev, data, SPS30_SAUL_VAL_IDX_MC_PM_1_2P5_4);
}

static int read_mc_pm_10(const void *dev, phydat_t *data)
{
    return _read(dev, data, SPS30_SAUL_VAL_IDX_MC_PM_10);
}

static int read_nc_pm_0p5_1_2p5(const void *dev, phydat_t *data)
{
    return _read(dev, data, SPS30_SAUL_VAL_IDX_NC_PM_0P5_1_2P5);
}

static int read_nc_pm_4_10(const void *dev, phydat_t *data)
{
    return _read(dev, data, SPS30_SAUL_VAL_IDX_NC_PM_4_10);
}

static int read_ps(const void *dev, phydat_t *data)
{
    return _read(dev, data, SPS30_SAUL_VAL_IDX_PS);
}

const saul_driver_t sps30_saul_driver_mc_pm_1_2p5_4 = {
    .read = read_mc_pm_1_2p5_4,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t sps30_saul_driver_mc_pm_10 = {
    .read = read_mc_pm_10,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t sps30_saul_driver_nc_pm_0p5_1_2p5 = {
    .read = read_nc_pm_0p5_1_2p5,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t sps30_saul_driver_nc_pm_4_10 = {
    .read = read_nc_pm_4_10,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t sps30_saul_driver_ps = {
    .read = read_ps,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_SIZE
};
