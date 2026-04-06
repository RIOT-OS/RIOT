/*
 * SPDX-FileCopyrightText: 2019 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sht2x
 * @{
 *
 * @file
 * @brief       SAUL adaption for Sensirion SHT20/SHT21/SHT25 devices
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include "saul.h"

#include "sht2x.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    res->val[0] = sht2x_read_temperature((const sht2x_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_relative_humidity(const void *dev, phydat_t *res)
{
    res->val[0] = sht2x_read_humidity((const sht2x_t *)dev);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t sht2x_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t sht2x_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM,
};
