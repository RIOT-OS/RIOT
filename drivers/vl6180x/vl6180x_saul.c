/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_vl6180x
 * @brief       VL6180X adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <string.h>

#include "saul.h"
#include "vl6180x.h"

#if IS_USED(MODULE_VL6180X_ALS)

static int read_als(const void *dev, phydat_t *res)
{
    if (vl6180x_als_data_ready((vl6180x_t*)dev) == VL6180X_OK &&
        vl6180x_als_read((vl6180x_t*)dev, NULL,
                         (uint16_t*)&res->val[0]) == VL6180X_OK) {
        res->unit = UNIT_LUX;
        res->scale = 0;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t vl6180x_saul_als_driver = {
    .read = read_als,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT,
};

#endif /* IS_USED(MODULE_VL6180X_ALS) */

#if IS_USED(MODULE_VL6180X_RNG)

static int read_rng(const void *dev, phydat_t *res)
{
    uint8_t mm;
    if (vl6180x_rng_data_ready((vl6180x_t*)dev) == VL6180X_OK) {
        vl6180x_rng_read((vl6180x_t*)dev, &mm);
        res->val[0] = mm;
        res->unit = UNIT_M;
        res->scale = -3;
        return 1;
    }
    return -ECANCELED;
}

const saul_driver_t vl6180x_saul_rng_driver = {
    .read = read_rng,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_DISTANCE,
};

#endif /* IS_USED(MODULE_VL6180X_RNG) */
