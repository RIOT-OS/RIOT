/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9150
 * @{
 *
 * @file
 * @brief       MPU9150 adaption to the RIOT actuator/sensor interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "mpu9150.h"

static int read_acc(const void *dev, phydat_t *res)
{
    int ret = mpu9150_read_accel((const mpu9150_t *)dev, (mpu9150_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -3;
    res->unit = UNIT_G;

    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    int ret = mpu9150_read_gyro((const mpu9150_t *)dev, (mpu9150_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -1;
    res->unit = UNIT_DPS;

    return 3;
}

static int read_mag(const void *dev, phydat_t *res)
{
    int ret = mpu9150_read_compass((const mpu9150_t *)dev, (mpu9150_results_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -2;
    res->unit = UNIT_GS;

    return 3;
}

const saul_driver_t mpu9150_saul_acc_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t mpu9150_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t mpu9150_saul_mag_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
