/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
    uint16_t mm;
    if (vl53l1x_data_ready((vl53l1x_t*)dev) == 0 &&
        vl53l1x_read_mm((vl53l1x_t*)dev, &mm) == 0) {
        res->val[0] = mm;
        res->unit = UNIT_M;
        res->scale = -3;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t vl53l1x_saul_driver = {
    .read = read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_DISTANCE,
};
