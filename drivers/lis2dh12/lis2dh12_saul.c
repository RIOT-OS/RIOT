/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lis2dh12
 * @{
 *
 * @file
 * @brief       LIS2DH12 accelerometer SAUL mapping
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "saul.h"
#include "lis2dh12.h"

static int read_accelerometer(const void *dev, phydat_t *res)
{
    if (lis2dh12_read(dev, (lis2dh12_fifo_data_t*)res->val) != LIS2DH12_OK) {
        return 0;
    }
    res->unit = UNIT_G_FORCE;
    res->scale = -3;
    return 3;
}

static int read_temperature(const void *dev, phydat_t *res)
{
    if (lis2dh12_read_temperature(dev, &res->val[0])) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

const saul_driver_t lis2dh12_saul_driver = {
    .read = read_accelerometer,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t lis2dh12_saul_temp_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
