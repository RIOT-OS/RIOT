/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_max31855
 * @{
 *
 * @file
 * @brief       SAUL adaption for MAX31855 devices
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "phydat.h"
#include "max31855.h"

static int read_temp(const void *dev, phydat_t *res, bool thermocouple)
{
    max31855_data_t data;
    max31855_read((max31855_t *)dev, &data);

    if (data.fault != MAX31855_FAULT_NO_FAULT) {
        return -ECANCELED;
    }

    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    if (thermocouple) {
        phydat_fit(res, &data.thermocouple_temperature, 1);
    }
    else {
        phydat_fit(res, &data.internal_temperature, 1);
    }

    return 1;
}

static int read_thermo_temp(const void *dev, phydat_t *res)
{
    return read_temp(dev, res, true);
}

static int read_internal_temp(const void *dev, phydat_t *res)
{
    return read_temp(dev, res, false);
}

const saul_driver_t max31855_thermo_temp_saul_driver = {
    .read = read_thermo_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t max31855_internal_temp_saul_driver = {
    .read = read_internal_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
