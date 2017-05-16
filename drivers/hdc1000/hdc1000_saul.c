/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_hdc1000
 * @{
 *
 * @file
 * @brief       HDC1000 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "hdc1000.h"

static int read_temp(void *dev, phydat_t *res)
{
    hdc1000_t *d = (hdc1000_t *)dev;

    if (hdc1000_read(d, &(res->val[0]), NULL) != HDC1000_OK) {
        return -ECANCELED;
    }
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_hum(void *dev, phydat_t *res)
{
    hdc1000_t *d = (hdc1000_t *)dev;

    if (hdc1000_read(d, NULL, &(res->val[0])) != HDC1000_OK) {
        return -ECANCELED;
    }
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t hdc1000_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t hdc1000_saul_hum_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
