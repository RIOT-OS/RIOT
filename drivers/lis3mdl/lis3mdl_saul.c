/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

    res->unit = UNIT_GAUSS;
    res->scale = -3;
    return 3;
}

const saul_driver_t lis3mdl_saul_mag_driver = {
    .read = read_mag,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_MAG,
};
