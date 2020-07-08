/*
 * Copyright (C) 2018 - 2020 Philipp-Alexander Blum
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
    hx711_t * device = (hx711_t *) dev;
    uint32_t result = hx711_get_units(device, device->params.read_times);
    res->val[0] = (int16_t) result;
    res->unit = UNIT_G;
    res->scale = 0;
    return 1;
}

static int tare_scale(const void *dev, phydat_t *data)
{
    uint8_t read_times = data->val[0];
    hx711_tare((hx711_t *)dev, read_times);
    return 1;
}

const saul_driver_t hx711_saul_driver = {
        .read = read_weight,
        .write = tare_scale,
        .type = SAUL_SENSE_SCALE
};
