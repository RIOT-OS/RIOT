/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lps331ap
 * @{
 *
 * @file
 * @brief       LPS331ap adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lps331ap.h"

static int read_pres(const void *dev, const uint8_t ctxt, phydat_t *res)
{
    (void)ctxt;

    res->val[0] = (int16_t)lps331ap_read_pres((const lps331ap_t *)dev);
    res->unit = UNIT_BAR;
    res->scale = -3;
    return 1;
}

static int read_temp(const void *dev, const uint8_t ctxt, phydat_t *res)
{
    (void)ctxt;

    res->val[0] = (int16_t)(lps331ap_read_temp((const lps331ap_t *)dev) / 10);
    res->unit = UNIT_TEMP_C;
    /* above division by ten leads to °C * 10^-2*/
    res->scale = -2;
    return 1;
}

const saul_driver_t lps331ap_saul_pres_driver = {
    .read = read_pres,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t lps331ap_saul_temp_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
