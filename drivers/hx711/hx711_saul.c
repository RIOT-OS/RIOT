/*
 * Copyright (C) 2019 Philipp-Alexander Blum
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hx711
 * @{
 *
 * @file
 * @brief       SAUL adaption for HX711 devices
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "hx711.h"
#include "hx711_params.h"

static int read_weight(const void *dev, phydat_t *res)
{
    (void)dev;
    uint32_t result = hx711_get_units(HX711_PARAM_AVG_TIMES);
    res->val[0] = (int16_t) result;
    res->unit = UNIT_G;
    res->scale = 0;
    return 1;
}

const saul_driver_t hx711_saul_driver = {
        .read = read_weight,
        .write = saul_notsup,
        .type = SAUL_SENSE_WEIGHT
};