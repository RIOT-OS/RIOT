/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_fxos8700
 * @{
 *
 * @file
 * @brief       FXOS8700 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "fxos8700.h"
#include "kernel_defines.h"

static int read_mag(const void *dev, phydat_t *res)
{
    if (fxos8700_read_cached(dev, NULL, (fxos8700_measurement_t *)res)
        != FXOS8700_OK) {
        /* Read failure */
        return -ECANCELED;
    }
    res->unit = UNIT_GS;
    res->scale = -3;
    return 3;
}

static int read_acc(const void *dev, phydat_t *res)
{
    if (fxos8700_read_cached(dev, (fxos8700_measurement_t *)res, NULL)
        != FXOS8700_OK) {
        /* Read failure */
        return -ECANCELED;
    }
    if (IS_ACTIVE(CONFIG_FXOS8700_USE_ACC_RAW_VALUES)) {
        res->unit = UNIT_NONE;
        res->scale = 0;
    }
    else {
        res->unit = UNIT_G;
        if (((fxos8700_t *)dev)->p.acc_range == FXOS8700_REG_XYZ_DATA_CFG_FS__2G) {
            res->scale = -4;
        }
        else {
            res->scale = -3;
        }
    }
    return 3;
}

const saul_driver_t fxos8700_saul_mag_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};

const saul_driver_t fxos8700_saul_acc_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};
