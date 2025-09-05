/*
 * SPDX-FileCopyrightText: 2018 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
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
    res->unit = UNIT_GAUSS;
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
        res->unit = UNIT_G_FORCE;
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_MAG,
};

const saul_driver_t fxos8700_saul_acc_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
