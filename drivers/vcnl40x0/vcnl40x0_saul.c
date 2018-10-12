/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @brief       VCNL40X0 adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "vcnl40x0.h"

static int read_als(const void *dev, phydat_t *res)
{
    uint16_t data;
    int ret = vcnl40x0_read_als((const vcnl40x0_t *)dev, &data);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->val[0] = data / 4;
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

const saul_driver_t vcnl40x0_saul_als_driver = {
    .read = read_als,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT,
};
