/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CO2
};
