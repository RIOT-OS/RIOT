/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_qmi8658
 * @{
 *
 * @file
 * @brief       SAUL implementation for the QMI8658 IMU
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 *
 * @}
 */

#include "qmi8658.h"
#include "saul.h"

static int read_acc(const void *dev, phydat_t *res)
{
    int ret = qmi8658_read_acc((const qmi8658_t *)dev, (qmi8658_3d_data_t *)res->val);

    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -3;
    res->unit = UNIT_G_FORCE;

    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    int ret = qmi8658_read_gyro((const qmi8658_t *)dev, (qmi8658_3d_data_t *)res->val);

    if (ret < 0) {
        return -ECANCELED;
    }

    res->scale = -1;
    res->unit = UNIT_DPS;

    return 3;
}

static int read_temp(const void *dev, phydat_t *res)
{
    if (qmi8658_read_temp((const qmi8658_t *)dev, &res->val[0]) < 0) {
        return -ECANCELED;
    }
    res->scale = -2;
    res->unit = UNIT_TEMP_C;

    return 1;
}

const saul_driver_t qmi8658_saul_acc_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t qmi8658_saul_gyro_driver = {
    .read = read_gyro,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_GYRO,
};

const saul_driver_t qmi8658_saul_temp_driver = {
    .read = read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
