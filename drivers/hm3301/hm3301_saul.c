/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hm3301
 * @{
 * @file
 * @brief       SAUL adaption of the HM3301 particulate matter sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "hm3301.h"
#include "hm3301_params.h"
#include "hm3301_constants.h"

static int read_mc_pm_1(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_HM3301_INDOOR_ENVIRONMENT)) {
        value = values.mc_pm_1;
    }
    else {
        value = values.amc_pm_1;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_mc_pm_2p5(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_HM3301_INDOOR_ENVIRONMENT)) {
        value = values.mc_pm_2p5;
    }
    else {
        value = values.amc_pm_2p5;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_mc_pm_10(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;
    if (IS_ACTIVE(CONFIG_HM3301_INDOOR_ENVIRONMENT)) {
        value = values.mc_pm_10;
    }
    else {
        value = values.amc_pm_10;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_1(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_1;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_2p5(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_2p5;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_10(const void *_dev, phydat_t *data)
{
    hm3301_t *dev = (hm3301_t *)_dev;

    hm3301_data_t values;
    hm3301_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_10;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

const saul_driver_t hm3301_saul_driver_mc_pm_1 = {
    .read = read_mc_pm_1,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t hm3301_saul_driver_mc_pm_2p5 = {
    .read = read_mc_pm_2p5,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t hm3301_saul_driver_mc_pm_10 = {
    .read = read_mc_pm_10,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t hm3301_saul_driver_nc_pm_1 = {
    .read = read_nc_pm_1,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t hm3301_saul_driver_nc_pm_2p5 = {
    .read = read_nc_pm_2p5,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t hm3301_saul_driver_nc_pm_10 = {
    .read = read_nc_pm_10,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};
