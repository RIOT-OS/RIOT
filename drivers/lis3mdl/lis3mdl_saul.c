/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lis3mdl
 * @{
 *
 * @file
 * @brief       LIS3MDL adaption to the RIOT actuator/sensor interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lis3mdl.h"

static int read_mag(const void *dev, phydat_t *res)
{
    const lis3mdl_t *d = (const lis3mdl_t *)dev;

    lis3mdl_read_mag(d, (lis3mdl_3d_data_t *)res);

    res->unit = UNIT_GS;
    res->scale = -3;
    return 3;
}

const saul_driver_t lis3mdl_saul_mag_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
