/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tsl4531x
 * @{
 *
 * @file
 * @brief       SAUL interface for TSL4531 Luminosity sensor.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include "saul.h"
#include "tsl4531x.h"

static int _read(const void *dev, phydat_t *res)
{
    int nvals;
    uint16_t lux;

    nvals = (tsl4531x_read(dev, &lux) >= 0)? 1 : 0;

    res->val[0] = lux;
    res->unit = UNIT_LUX;
    res->scale = 0;
    return nvals;
}

const saul_driver_t tsl2561_illuminance_saul_driver = {
    .read = _read,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};
