/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_veml6070
 * @{
 *
 * @file
 * @brief       SAUL adaption for VEML6070 UV sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "veml6070.h"
#include "xtimer.h"

static int read_uv(void *dev, phydat_t *res)
{
    veml6070_t *d = (veml6070_t *)dev;

    res->val[0] = veml6070_read_uv(d);
    res->unit = UNIT_NONE;
    res->scale = -1;
    return 1;
}

const saul_driver_t veml6070_uv_saul_driver = {
    .read = read_uv,
    .write = saul_notsup,
    .type = SAUL_SENSE_ANY
};
