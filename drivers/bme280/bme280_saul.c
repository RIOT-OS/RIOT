/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_bme280
 * @{
 *
 * @file
 * @brief       SAUL adoption for BME280 sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include "saul.h"

#include "bme280.h"

static int read_temperature(void *dev, phydat_t *res)
{
    bme280_t *d = (bme280_t *)dev;

    res->val[0] = bme280_read_temperature(d);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_relative_humidity(void *dev, phydat_t *res)
{
    bme280_t *d = (bme280_t *)dev;

    res->val[0] = bme280_read_humidity(d);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

static int read_pressure(void *dev, phydat_t *res)
{
    bme280_t *d = (bme280_t *)dev;

    res->val[0] = bme280_read_pressure(d) / 100;
    res->unit = UNIT_PA;
    res->scale = 2;

    return 1;
}

const saul_driver_t bme280_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t bme280_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};

const saul_driver_t bme280_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS,
};
