/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_htu21d
 * @brief       HTU21D adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "htu21d.h"

static int read_hum(const void *dev, phydat_t *res)
{
    int ret = htu21d_read((htu21d_t *)dev, 0, res->val, 0, 0);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_PERCENT;
    res->scale = -2;
    return 1;
}

static int read_tmp(const void *dev, phydat_t *res)
{
    int ret = htu21d_read((htu21d_t *)dev, res->val, 0, 0, 0);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t htu21d_saul_hum_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};

const saul_driver_t htu21d_saul_tmp_driver = {
    .read = read_tmp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
