/*
 * Copyright (C) 2017 Inria
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9x50
 * @{
 *
 * @file
 * @brief       MPU9X50 adaption to the RIOT actuator/sensor interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "mpu9x50.h"

static int read_acc(const void *dev, phydat_t *res)
{
    int ret = mpu9x50_read_accel((const mpu9x50_t *)dev, (mpu9x50_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -3;
    res->unit = UNIT_G;

    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    int ret = mpu9x50_read_gyro((const mpu9x50_t *)dev, (mpu9x50_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -1;
    res->unit = UNIT_DPS;

    return 3;
}

static int read_mag(const void *dev, phydat_t *res)
{
    int ret = mpu9x50_read_compass((const mpu9x50_t *)dev, (mpu9x50_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -2;
    res->unit = UNIT_GS;

    return 3;
}

const saul_driver_t mpu9x50_saul_acc_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t mpu9x50_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t mpu9x50_saul_mag_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
