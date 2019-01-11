/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_lsm6dsl
 * @{
 *
 * @file
 * @brief       SAUL implementation for the LSM6DSL 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "lsm6dsl.h"
#include "saul.h"

static int read_acc(const void *dev, phydat_t *res)
{
    int ret = lsm6dsl_read_acc((const lsm6dsl_t *)dev, (lsm6dsl_3d_data_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -3;
    res->unit = UNIT_G;

    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    int ret = lsm6dsl_read_gyro((const lsm6dsl_t *)dev, (lsm6dsl_3d_data_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -1;
    res->unit = UNIT_DPS;

    return 3;
}

static int read_temp(const void *dev, phydat_t *res)
{
    if (lsm6dsl_read_temp((const lsm6dsl_t *)dev, &res->val[0]) < 0) {
        return -ECANCELED;
    }
    res->scale = -2;
    res->unit = UNIT_TEMP_C;

    return 1;
}

const saul_driver_t lsm6dsl_saul_acc_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t lsm6dsl_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t lsm6dsl_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
