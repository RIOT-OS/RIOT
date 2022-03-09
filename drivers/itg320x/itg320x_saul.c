/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_itg320x
 * @brief       ITG320X adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "itg320x.h"

static int read_gyro(const void *dev, phydat_t *res)
{
    itg320x_data_t data;
    int ret = itg320x_read((const itg320x_t *)dev, &data);
    if (ret < 0) {
        return -ECANCELED;
    }
    /* convert milli-dps to deci-dps */
    res->val[0] = data.x;
    res->val[1] = data.y;
    res->val[2] = data.z;

    res->unit = UNIT_DPS;
    res->scale = -1;
    return 3;
}

static int read_temp(const void *dev, phydat_t *res)
{
    int ret = itg320x_read_temp((const itg320x_t *)dev, &res->val[0]);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -1;
    return 1;
}

const saul_driver_t itg320x_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t itg320x_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
