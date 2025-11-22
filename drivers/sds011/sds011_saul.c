/*
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sds011
 * @{
 *
 * @file
 * @brief       SAUL adaption for SDS011 sensor
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "sds011.h"

static int _read(const void *dev, phydat_t *res)
{
    sds011_data_t data;

    if (sds011_read((sds011_t *)dev, &data) == SDS011_OK) {
       res->val[0] = data.pm_2_5;
       res->val[1] = data.pm_10;
       res->unit = UNIT_GPM3;
       res->scale = -7;
       return 2;
    }
    return -ECANCELED;
}

const saul_driver_t sds011_saul_driver = {
    .read = _read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM
};
