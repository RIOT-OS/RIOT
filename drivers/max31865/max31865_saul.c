/*
 * Copyright (C) 2025 David Picard
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    max31865_data_t data;

    max31865_read((max31865_t *)dev, &data);

    if (data.fault != MAX31865_FAULT_NO_FAULT) {
        return -ECANCELED;
    }

    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    phydat_fit(res, &data.rtd_temperature_cdegc, 1);

    return 1;
}

const saul_driver_t max31865_saul_driver_temp = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
