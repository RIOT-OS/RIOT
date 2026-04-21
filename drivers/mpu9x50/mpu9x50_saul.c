/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
    res->unit = UNIT_G_FORCE;

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
    res->unit = UNIT_GAUSS;

    return 3;
}

const saul_driver_t mpu9x50_saul_acc_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t mpu9x50_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t mpu9x50_saul_mag_driver = {
    .read = read_mag,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_MAG,
};
