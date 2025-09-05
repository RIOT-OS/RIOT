/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sgp30
 * @brief       SAUL adaption for SGP30 sensor
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "phydat.h"
#include "saul.h"

#include "sgp30.h"

static int _read_tvoc(const void *dev, phydat_t *res)
{
    sgp30_data_t data;

    if (sgp30_read_measurements((sgp30_t *)dev, &data)) {
        return -ECANCELED;
    }
    res->val[0] = data.tvoc;
    res->unit = UNIT_PPB;
    res->scale = 0;
    return 1;
}

static int _read_eco2(const void *dev, phydat_t *res)
{
    sgp30_data_t data;

    if (sgp30_read_measurements((sgp30_t *)dev, &data)) {
        return -ECANCELED;
    }
    res->val[0] = data.eco2;
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

const saul_driver_t sgp30_saul_driver_eco2 = {
    .read = _read_eco2,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CO2
};

const saul_driver_t sgp30_saul_driver_tvoc = {
    .read = _read_tvoc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TVOC
};
