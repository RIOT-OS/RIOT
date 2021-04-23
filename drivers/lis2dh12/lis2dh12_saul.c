/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
    res->unit = UNIT_G;
    res->scale = -3;
    return 3;
}

const saul_driver_t lis2dh12_saul_driver = {
    .read = read_accelerometer,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL
};
