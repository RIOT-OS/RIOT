/*
 * Copyright (C) 2020 Viktor Gal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hdc2010
 * @{
 *
 * @file
 * @brief       HDC2010 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "hdc2010.h"

static int read_temp(const void *dev, phydat_t *res)
{
    if (hdc2010_read((hdc2010_t *)dev, &(res->val[0]), NULL) != HDC2010_OK) {
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_hum(const void *dev, phydat_t *res)
{
    if (hdc2010_read((hdc2010_t *)dev, NULL, &(res->val[0])) != HDC2010_OK) {
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t hdc2010_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t hdc2010_saul_hum_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
