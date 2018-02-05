/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hts221
 * @{
 *
 * @file
 * @brief       HTS221 adaption to the RIOT SAUL interface
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "hts221.h"

static int read_temp(const void *dev, phydat_t *res)
{
    if (hts221_read_temperature((const hts221_t *)dev, &res->val[0]) != HTS221_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -1;

    return 1;
}

static int read_hum(const void *dev, phydat_t *res)
{
    if (hts221_read_humidity((const hts221_t *)dev, (uint16_t *)&res->val[0]) != HTS221_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -1;

    return 1;
}

const saul_driver_t hts221_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t hts221_saul_hum_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
