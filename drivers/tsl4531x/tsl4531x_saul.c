/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT
};
