/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ad7746
 * @{
 *
 * @file
 * @brief       AD7746 adaption to the RIOT actuator/sensor interface
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "ad7746.h"

static int _read_cap(const void *dev, phydat_t *res)
{
    int val;
    if (ad7746_read_capacitance_1((ad7746_t *)dev, &val)) {
        return -ECANCELED;
    }

    res->val[0] = val;
    res->unit = UNIT_F;
    res->scale = -15;

    return 1;
}

static int _read_temp(const void *dev, phydat_t *res)
{
    int val;
    int result;
    do {
        result = ad7746_read_temperature_int((ad7746_t *)dev, &val);
    } while (result == AD7746_NODATA);

    if (result == AD7746_OK) {
        res->val[0] = val;
        res->unit = UNIT_TEMP_C;
        res->scale = 0;
        return 1;
    }
    else {
        return -ECANCELED;
    }
}

static int _read_volt(const void *dev, phydat_t *res)
{
    int val;
    int result;
    do {
        result = ad7746_read_voltage_in((ad7746_t *)dev, &val);
    } while (result == AD7746_NODATA);

    if (result == AD7746_OK) {
        res->val[0] = val;
        res->unit = UNIT_V;
        res->scale = -3;
        return 1;
    }
    else {
        return -ECANCELED;
    }
}

const saul_driver_t ad7746_saul_driver_cap = {
    .read = _read_cap,
    .write = saul_notsup,
    .type = SAUL_SENSE_CAPACITANCE,
};

const saul_driver_t ad7746_saul_driver_temp = {
    .read = _read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t ad7746_saul_driver_volt = {
    .read = _read_volt,
    .write = saul_notsup,
    .type = SAUL_SENSE_VOLTAGE
};
