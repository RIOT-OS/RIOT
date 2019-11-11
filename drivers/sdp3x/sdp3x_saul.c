/*
 * Copyright (C) 2019 Jan Schlichter
 *               2020 Nishchay Agrawal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sdp3x
 * @{
 * @file
 * @brief       SAUL adaption for Sensirion SDP3x devices
 *
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 *
 * @}
 */

#include "saul.h"

#include "sdp3x.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    int32_t temp = sdp3x_read_single_temperature((sdp3x_t *)dev,
                                                SDP3X_FLAG_DIFF_PRESS);

    /* Fit 32 bit data into 16 bit fields of phydat_t */
    phydat_fit(res, &temp, 1);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

static int read_differential_pressure(const void *dev, phydat_t *res)
{
    int32_t pres = sdp3x_read_single_differential_pressure((sdp3x_t *)dev,
                                                        SDP3X_FLAG_DIFF_PRESS);

    /* Fit 32 bit data into 16 bit fields of phydat_t */
    phydat_fit(res, &pres, 1);
    res->unit = UNIT_PA;
    res->scale = -2;
    return 1;
}

const saul_driver_t sdp3x_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t sdp3x_differential_pressure_saul_driver = {
    .read = read_differential_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS,
};
