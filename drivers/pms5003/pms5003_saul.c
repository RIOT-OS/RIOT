/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pms5003
 * @{
 * @file
 * @brief       SAUL adaption of the PMS5003 particulate matter sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "pms5003.h"
#include "pms5003_params.h"
#include "pms5003_constants.h"
#include "ztimer.h"

static pms5003_data_t values;

static void _get_data(pms5003_t *dev)
{
    pms5003_read_measurement(dev, &values);
}

static int read_mc_pm_1(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_PMS5003_INDOOR_ENVIRONMENT)) {
        value = values.values.mc_pm_1;
    }
    else {
        value = values.values.amc_pm_1;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_mc_pm_2p5(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_PMS5003_INDOOR_ENVIRONMENT)) {
        value = values.values.mc_pm_2p5;
    }
    else {
        value = values.values.amc_pm_2p5;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_mc_pm_10(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_PMS5003_INDOOR_ENVIRONMENT)) {
        value = values.values.mc_pm_10;
    }
    else {
        value = values.values.amc_pm_10;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_1(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.values.nc_pm_1;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_2p5(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.values.nc_pm_2p5;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_10(const void *_dev, phydat_t *data)
{
    pms5003_t *dev = (pms5003_t *)_dev;

    _get_data(dev);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.values.nc_pm_10;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

const saul_driver_t pms5003_saul_driver_mc_pm_1 = {
    .read = read_mc_pm_1,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t pms5003_saul_driver_mc_pm_2p5 = {
    .read = read_mc_pm_2p5,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t pms5003_saul_driver_mc_pm_10 = {
    .read = read_mc_pm_10,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t pms5003_saul_driver_nc_pm_1 = {
    .read = read_nc_pm_1,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t pms5003_saul_driver_nc_pm_2p5 = {
    .read = read_nc_pm_2p5,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t pms5003_saul_driver_nc_pm_10 = {
    .read = read_nc_pm_10,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};
