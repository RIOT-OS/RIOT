/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gp2y10xx
 * @{
 *
 * @file
 * @brief       GP2Y10xxAU0F adaption to the RIOT actuator/sensor interface
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "gp2y10xx.h"

static int _read(const void *dev, phydat_t *res)
{
    uint16_t val;
    if (gp2y10xx_read_density((const gp2y10xx_t *)dev, &val) != GP2Y10XX_OK) {
        return -ECANCELED;
    }

    /* ug/m3 so it's g/m3 with a -6 scale */
    res->unit = UNIT_GPM3;
    res->scale = -6;
    res->val[0] = val;

    return 1;
}

const saul_driver_t gp2y10xx_saul_driver = {
    .read = _read,
    .write = saul_notsup,
    .type = SAUL_SENSE_PM,
};
