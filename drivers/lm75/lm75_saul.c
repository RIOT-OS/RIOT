/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lm75
 * @{
 *
 * @file
 * @brief       SAUL adaption for lm75 compatible device
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lm75.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    int temperature;

    lm75_get_temperature((lm75_t *)dev, &temperature);
    res->val[0] = temperature / 10; /* phydat_t is 16 bit */
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t lm75_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};
