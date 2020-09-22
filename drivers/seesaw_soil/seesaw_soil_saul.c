/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_seesaw_soil
 * @{
 *
 * @file
 * @brief       Seesaw Soil adaption to the RIOT actuator/sensor interface
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "seesaw_soil.h"

static int read_temp(const void *dev, phydat_t *res)
{
    if (seesaw_soil_temperature((const seesaw_soil_t *)dev, &(res->val[0])) != SEESAW_SOIL_OK) {
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

const saul_driver_t seesaw_soil_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
