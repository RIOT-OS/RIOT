/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_shtcx
 * @{
 *
 * @file
 * @brief       SHTCX adaption to the RIOT actuator/sensor interface
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

#include "saul.h"
#include "shtcx.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    if (shtcx_read((shtcx_t *)dev, NULL, &res->val[0]) != SHTCX_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_relative_humidity(const void *dev, phydat_t *res)
{
    if (shtcx_read((shtcx_t *)dev, (uint16_t *)&res->val[0], NULL) != SHTCX_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t shtcx_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t shtcx_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM,
};
