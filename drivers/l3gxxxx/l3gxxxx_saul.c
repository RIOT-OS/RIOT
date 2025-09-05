/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_l3gxxxx
 * @brief       L3Gxxxx adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "l3gxxxx.h"

static int read(const void *dev, phydat_t *res)
{
    l3gxxxx_data_t data;
    int ret = l3gxxxx_read((const l3gxxxx_t *)dev, &data);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->val[0] = data.x / 100;
    res->val[1] = data.y / 100;
    res->val[2] = data.z / 100;
    res->unit = UNIT_DPS;
    res->scale = -1;

    return 3;
}

const saul_driver_t l3gxxxx_saul_driver = {
    .read = read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_GYRO,
};
