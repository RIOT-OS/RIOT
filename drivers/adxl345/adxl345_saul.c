/*
 * Copyright (C) 2017 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

    res->unit = UNIT_G;
    res->scale = -3;
    return 3;
}

const saul_driver_t adxl345_saul_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};
