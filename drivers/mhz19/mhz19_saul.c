/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mhz19
 * @{
 *
 * @file
 * @brief       SAUL adaption for MH-Z19 CO2 sensor device
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "saul.h"
#include "mhz19.h"

static int read_ppm(const void *dev, phydat_t *res)
{
    int16_t ppm;

    /* Drops the const keyword, otherwise the mutex can't be locked */
    if (mhz19_get_ppm((mhz19_t *)dev, &ppm) < 0) {
        return -ECANCELED;
    }
    res->val[0] = ppm;
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

const saul_driver_t mhz19_ppm_saul_driver = {
    .read = read_ppm,
    .write = saul_notsup,
    .type = SAUL_SENSE_CO2
};
