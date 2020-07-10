/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l3gd20h
 * @brief       L3GD20H adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "l3gd20h.h"

static int read(const void *dev, phydat_t *res)
{
    l3gd20h_data_t data;
    int ret = l3gd20h_read((const l3gd20h_t *)dev, &data);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->val[0] = data.x;
    res->val[1] = data.y;
    res->val[2] = data.z;
    res->unit = UNIT_DPS;
    res->scale = -3;
    return 3;
}

const saul_driver_t l3gd20h_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};
