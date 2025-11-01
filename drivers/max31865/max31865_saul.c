/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_max31865
 * @{
 * @file
 * @brief       SAUL adaption for MAX31865 devices
 * @author      David Picard
 * @}
 */

#include <string.h>

#include "saul.h"
#include "phydat.h"
#include "max31865.h"
#include "stdio.h"

static int read_temp(const void *dev, phydat_t *res)
{
    int32_t temperature_cdegc;

    if (max31865_read(dev, &temperature_cdegc) != 0) {
        return -ECANCELED;
    }

    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    phydat_fit(res, &temperature_cdegc, 1);

    return 1;
}

const saul_driver_t max31865_saul_driver_temp = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
