/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_bmx280
 * @{
 *
 * @file
 * @brief       SAUL adoption for BMX280 sensors (BME280 and BMP280).
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "saul.h"

#include "bmx280.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    res->val[0] = bmx280_read_temperature((bmx280_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_pressure(const void *dev, phydat_t *res)
{
    res->unit = UNIT_PA;
    res->scale = 0;

    int32_t val = bmx280_read_pressure((bmx280_t *)dev);
    phydat_fit(res, &val, 1);

    return 1;
}

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
static int read_relative_humidity(const void *dev, phydat_t *res)
{
    res->val[0] = bme280_read_humidity((bmx280_t *)dev);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}
#endif

const saul_driver_t bmx280_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t bmx280_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PRESS,
};

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
const saul_driver_t bme280_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM,
};
#endif
