/*
 * SPDX-FileCopyrightText: 2017 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_adxl345
 * @{
 *
 * @file
 * @brief       SAUL adaption for ADXL345 device
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "saul.h"
#include "adxl345.h"

static int read_acc(const void *dev, phydat_t *res)
{
    adxl345_read((const adxl345_t *)dev, (adxl345_data_t *)res->val);

    res->unit = UNIT_G_FORCE;
    res->scale = -3;
    return 3;
}

const saul_driver_t adxl345_saul_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
