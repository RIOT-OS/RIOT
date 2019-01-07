/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

    res->unit = UNIT_GS;
    res->scale = 2;

    return 3;
}

const saul_driver_t mag3110_saul_driver = {
    .read = read_mag,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
