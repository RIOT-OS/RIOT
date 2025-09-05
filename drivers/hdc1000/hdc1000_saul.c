/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hdc1000
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

static int read_temp(const void *dev, phydat_t *res)
{
    if (hdc1000_read_cached((const hdc1000_t *)dev, &(res->val[0]), NULL) != HDC1000_OK) {
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
    if (hdc1000_read_cached((const hdc1000_t *)dev, NULL, &(res->val[0])) != HDC1000_OK) {
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t hdc1000_saul_temp_driver = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t hdc1000_saul_hum_driver = {
    .read = read_hum,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM,
};
