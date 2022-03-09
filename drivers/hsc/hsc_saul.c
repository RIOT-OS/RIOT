/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hsc
 * @{
 *
 * @file
 * @brief       SAUL adaption for HSC device
 *
 * @author      Quang Pham <phhr_quang@live.com>
 * @}
 */

#include <string.h>
#include "saul.h"
#include "hsc.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    if (hsc_read_temperature(dev, res->val) != 0) {
        return 0;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -1;
    return 1;
}

static int read_pressure(const void *dev, phydat_t *res)
{
    int32_t value;
    if (hsc_read_pressure(dev, &value) != 0) {
        return 0;
    }
    res->unit = UNIT_BAR;
    res->scale = -6;
    phydat_fit(res, &value, 1);
    return 1;
}

const saul_driver_t hsc_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t hsc_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS
};
