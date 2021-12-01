/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @{
 *
 * @file
 * @brief       SAUL adaption for VCNL40X0 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "vcnl40x0.h"
#include "vcnl40x0_params.h"

static int read_proximity(const void *dev, phydat_t *res)
{
    const vcnl40x0_t *d = (vcnl40x0_t *)dev;

    res->val[0] = (int16_t)vcnl40x0_read_proximity(d);
    res->unit = UNIT_CTS;
    res->scale = 0;
    return 1;
}

static int read_illuminance(const void *dev, phydat_t *res)
{
    const vcnl40x0_t *d = (vcnl40x0_t *)dev;

    res->val[0] = (int16_t)vcnl40x0_read_illuminance(d);
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

const saul_driver_t vcnl40x0_proximity_saul_driver = {
    .read = read_proximity,
    .write = saul_notsup,
    .type = SAUL_SENSE_PROXIMITY
};

const saul_driver_t vcnl40x0_illuminance_saul_driver = {
    .read = read_illuminance,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};
