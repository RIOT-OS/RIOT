/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mag3110
 * @{
 *
 * @file
 * @brief       MAG3110 adaption to the RIOT actuator/sensor interface
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "mag3110.h"

static int read_mag(const void *dev, phydat_t *res)
{
    mag3110_read((const mag3110_t *)dev, (mag3110_data_t *)res->val);

    res->unit = UNIT_GAUSS;
    res->scale = 2;

    return 3;
}

const saul_driver_t mag3110_saul_driver = {
    .read = read_mag,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_MAG,
};
