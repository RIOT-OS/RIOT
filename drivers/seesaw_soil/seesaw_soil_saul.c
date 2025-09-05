/*
 * SPDX-FileCopyrightText: 2020 Viktor Gal
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
