/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vl53l1x
 * @brief       VL53L1X adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "vl53l1x.h"

static int read(const void *dev, phydat_t *res)
{
    if (vl53l1x_data_ready((vl53l1x_t*)dev) == VL53L1X_OK &&
        vl53l1x_read_mm((vl53l1x_t*)dev, &res->val[0]) == VL53L1X_OK) {
        res->unit = UNIT_M;
        res->scale = -3;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t vl53l1x_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_DISTANCE,
};
