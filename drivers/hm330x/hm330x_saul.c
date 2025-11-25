/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hm330x
 * @{
 * @file
 * @brief       SAUL adaption of the HM330X particulate matter sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "hm330x.h"
#include "hm330x_params.h"
#include "hm330x_constants.h"

static int read_mc_pm_1(const void *_dev, phydat_t *data)
{
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;

    if (IS_ACTIVE(CONFIG_HM330X_INDOOR_ENVIRONMENT)) {
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
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;

    if (IS_ACTIVE(CONFIG_HM330X_INDOOR_ENVIRONMENT)) {
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
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value;

    if (IS_ACTIVE(CONFIG_HM330X_INDOOR_ENVIRONMENT)) {
        value = values.mc_pm_10;
    }
    else {
        value = values.amc_pm_10;
    }
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

#if IS_USED(MODULE_HM3302)
static int read_nc_pm_1(const void *_dev, phydat_t *data)
{
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_1;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_2p5(const void *_dev, phydat_t *data)
{
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_2p5;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_nc_pm_10(const void *_dev, phydat_t *data)
{
    hm330x_t *dev = (hm330x_t *)_dev;

    hm330x_data_t values;

    hm330x_read(dev, &values);
    data->unit = UNIT_CPM3;
    data->scale = 4;
    uint32_t value = values.nc_pm_10;

    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}
#endif

const saul_driver_t hm330x_saul_driver_mc_pm_1 = {
    .read = read_mc_pm_1,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t hm330x_saul_driver_mc_pm_2p5 = {
    .read = read_mc_pm_2p5,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};

const saul_driver_t hm330x_saul_driver_mc_pm_10 = {
    .read = read_mc_pm_10,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};

#if IS_USED(MODULE_HM3302)
const saul_driver_t hm330x_saul_driver_nc_pm_1 = {
    .read = read_nc_pm_1,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t hm330x_saul_driver_nc_pm_2p5 = {
    .read = read_nc_pm_2p5,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT
};

const saul_driver_t hm330x_saul_driver_nc_pm_10 = {
    .read = read_nc_pm_10,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COUNT
};
#endif
