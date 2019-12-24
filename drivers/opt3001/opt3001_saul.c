/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_opt3001
 * @{
 *
 * @file
 * @brief       OPT3001 adaption to the RIOT actuator/sensor interface
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "opt3001.h"

static int read_lux(const void *dev, phydat_t *res)
{
    uint32_t convlux;

    if (opt3001_read_lux((const opt3001_t *)dev, &convlux) != OPT3001_OK) {
        return -ECANCELED;
    }

    res->val[0] = convlux / 1000;
    res->val[1] = 0;
    res->val[2] = 0;

    res->unit = UNIT_LUX;
    res->scale = 0;

     return 1;
 }

const saul_driver_t opt3001_saul_driver = {
    .read = read_lux,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT,
};
