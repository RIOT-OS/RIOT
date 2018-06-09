/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
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
 * @brief       SAUL adaption for the Zhengzhou Winsen Electronics Technology MH-Z19 CO2 sensor
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include "mhz19.h"
#include "saul.h"

static int read_co2_ppm(const void *dev, phydat_t *res)
{
    mhz19_t *d = (mhz19_t *)dev;
    int16_t val = mhz19_get_ppm(d);

    if (val < 0) {
        return -ECANCELED;
    }

    res->val[0] = val;
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

const saul_driver_t mhz19_co2_saul_driver = {
    .read = read_co2_ppm,
    .write = saul_notsup,
    .type = SAUL_SENSE_CO2,
};
