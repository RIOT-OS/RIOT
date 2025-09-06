/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_io1_xplained
 * @{
 *
 * @file
 * @brief       SAUL adaption for IO1 Xplained extension
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "io1_xplained.h"
#include "at30tse75x.h"

static float temperature;

static int read_temperature(const void *dev, phydat_t *res)
{
    const io1_xplained_t *io1 = (const io1_xplained_t *)dev;
    at30tse75x_get_temperature(&io1->temp, &temperature);
    res->val[0] = (int)(temperature * 100.0);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t io1_xplained_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
