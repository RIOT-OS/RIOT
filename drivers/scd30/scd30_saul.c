/*
 * Copyright (C) 2020 Technische Universität Braunschweig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_scd30
 * @file
 * @brief       SAUL adaption for Sensirion SCD30 sensor
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author		Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 */

#include "saul.h"
#include "scd30.h"
#include "scd30_internal.h"

static void _float_fit(float src, phydat_t *data, uint32_t mul)
{
    int32_t i32;

    i32 = src * mul;
    phydat_fit(data, &i32, 1);
}

static int _co2_read(const void *dev, phydat_t *res)
{
    int ret = 0;
    scd30_measurement_t result;

    scd30_set_param(dev, SCD30_START, SCD30_DEF_PRESSURE);

    ret = scd30_read_triggered((scd30_t *)dev, &result);
    if (ret != 0) {
        return ret;
    }
    res->scale = -2;
    res->unit = UNIT_PPM;
    _float_fit(result.co2_concentration, res, 100);

    scd30_stop_measurements(dev);
    return 1;
}

static int _temp_read(const void *dev, phydat_t *res)
{
    int ret = 0;
    scd30_measurement_t result;

    scd30_set_param(dev, SCD30_START, SCD30_DEF_PRESSURE);

    ret = scd30_read_triggered((scd30_t *)dev, &result);
    if (ret != 0) {
        return ret;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    _float_fit(result.temperature, res, 100);

    scd30_stop_measurements(dev);
    return 1;
}

static int _hum_read(const void *dev, phydat_t *res)
{
    int ret = 0;
    scd30_measurement_t result;

    scd30_set_param(dev, SCD30_START, SCD30_DEF_PRESSURE);

    ret = scd30_read_triggered((scd30_t *)dev, &result);
    if (ret != 0) {
        return ret;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -2;
    _float_fit(result.relative_humidity, res, 100);

    scd30_stop_measurements(dev);
    return 1;
}

const saul_driver_t scd30_co2_saul_driver = {
    .read = _co2_read,
    .write = saul_notsup,
    .type = SAUL_SENSE_CO2,
};

const saul_driver_t scd30_temp_saul_driver = {
    .read = _temp_read,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t scd30_hum_saul_driver = {
    .read = _hum_read,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
