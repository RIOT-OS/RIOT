/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @{
 *
 * @file
 * @brief       SAUL adaption for CCS811 device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "ccs811.h"

static int read_eco2(const void *dev, phydat_t *res)
{
    if (ccs811_read_eco2((const ccs811_t *)dev, (uint16_t *)res->val) < 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

static int read_tvoc(const void *dev, phydat_t *res)
{
    if (ccs811_read_tvoc((const ccs811_t *)dev, (uint16_t *)res->val) < 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_PPB;
    res->scale = 0;
    return 1;
}

const saul_driver_t ccs811_eco2_saul_driver = {
    .read = read_eco2,
    .write = saul_notsup,
    .type = SAUL_SENSE_CO2
};

const saul_driver_t ccs811_tvoc_saul_driver = {
    .read = read_tvoc,
    .write = saul_notsup,
    .type = SAUL_SENSE_TVOC
};
