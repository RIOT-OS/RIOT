/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hmc5883l
 * @brief       HMC5883L adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "hmc5883l.h"

static int read(const void *dev, phydat_t *res)
{
    hmc5883l_data_t data;
    int ret = hmc5883l_read((const hmc5883l_t *)dev, &data);
    if (ret < 0) {
        return -ECANCELED;
    }
    res->val[0] = data.x;
    res->val[1] = data.y;
    res->val[2] = data.z;
    res->unit = UNIT_GS;
    res->scale = -3;
    return 3;
}

const saul_driver_t hmc5883l_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
