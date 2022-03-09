/*
 * Copyright (C) 2018 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
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
    .write = saul_notsup,
    .type = SAUL_SENSE_PM
};
