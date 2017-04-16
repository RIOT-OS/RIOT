/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

static float temperature;

static int read_temperature(void *dev, phydat_t *res)
{
    io1_xplained_t *d = (io1_xplained_t *)dev;
    io1_xplained_read_temperature(d, &temperature);
    res->val[0] = (int)(temperature * 100.0);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t io1_xplained_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};
