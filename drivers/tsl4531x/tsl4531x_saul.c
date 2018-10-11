/*
 * Copyright (C) 2016 Inria
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
 * @brief       SAUL interface for TSL4531x Luminosity sensor.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 * @author      Daniel Petry <daniel.petry@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include "saul.h"
#include "tsl4531x.h"

static int _read(const void *dev, phydat_t *data)
{
    memset(data, 0, sizeof(phydat_t));
    data->val[0] = tsl4531x_simple_read(*(tsl4531x_t * const*)dev);
    data->unit = UNIT_LUX;
    data->scale = 0;
    return 1;
}

const saul_driver_t tsl4531x_saul_driver = {
    .read = _read,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};
