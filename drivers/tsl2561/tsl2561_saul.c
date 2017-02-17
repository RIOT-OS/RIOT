/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tsl2561
 * @{
 *
 * @file
 * @brief       SAUL adaption for TSL2561 device
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "saul.h"
#include "tsl2561.h"
#include "xtimer.h"

static int read_illuminance(void *dev, phydat_t *res)
{
    tsl2561_t *d = (tsl2561_t *)dev;

    res->val[0] = tsl2561_read_illuminance(d);
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

const saul_driver_t tsl2561_illuminance_saul_driver = {
    .read = read_illuminance,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};
