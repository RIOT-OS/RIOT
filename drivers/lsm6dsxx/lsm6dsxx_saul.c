/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2024 HAW Hamburg.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lsm6dsxx
 * @{
 *
 * @file
 * @brief       SAUL implementation for the LSM6DSXX 3D accelerometer/gyroscope.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Miquel Borrell <miquel.borrell@haw-hamburg.de>
 *
 * @}
 */

#include "lsm6dsxx.h"
#include "saul.h"

static int read_acc(const void *dev, phydat_t *res)
{
    int ret = lsm6dsxx_read_acc((const lsm6dsxx_t *)dev, (lsm6dsxx_3d_data_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -3;
    res->unit = UNIT_G_FORCE;

    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    int ret = lsm6dsxx_read_gyro((const lsm6dsxx_t *)dev, (lsm6dsxx_3d_data_t *)res->val);
    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -1;
    res->unit = UNIT_DPS;

    return 3;
}

static int read_temp(const void *dev, phydat_t *res)
{
    if (lsm6dsxx_read_temp((const lsm6dsxx_t *)dev, &res->val[0]) < 0) {
        return -ECANCELED;
    }
    res->scale = -2;
    res->unit = UNIT_TEMP_C;

    return 1;
}

const saul_driver_t lsm6dsxx_saul_acc_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t lsm6dsxx_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t lsm6dsxx_saul_temp_driver = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
