/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmx055
 * @{
 *
 * @file
 * @brief       bmx055 adaption to the RIOT actuator/sensor interface
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "saul.h"
#include "bmx055.h"

static int read_mag(const void *dev, phydat_t *res)
{
    const bmx055_t *mydev = (const bmx055_t *)dev;

    if (bmx055_mag_read(mydev, res->val) != BMX055_OK) {
        return 0;
    }
    res->unit = UNIT_GS;
    res->scale = 0;
    return 3;
}

static int read_acc(const void *dev, phydat_t *res)
{
    const bmx055_t *mydev = (const bmx055_t *)dev;

    if (bmx055_acc_read(mydev, res->val) != BMX055_OK) {
        return 0;
    }
    res->unit = UNIT_G;
    res->scale = -3;
    return 3;
}

static int read_gyro(const void *dev, phydat_t *res)
{
    const bmx055_t *mydev = (const bmx055_t *)dev;

    if (bmx055_gyro_read(mydev, res->val) != BMX055_OK) {
        return 0;
    }
    res->unit = UNIT_DPS;
    res->scale = 0;
    return 3;
}

const saul_driver_t bmx055_magnetometer_saul_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};

const saul_driver_t bmx055_accelerometer_saul_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t bmx055_gyroscope_saul_driver = {
    .read = read_gyro,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};
