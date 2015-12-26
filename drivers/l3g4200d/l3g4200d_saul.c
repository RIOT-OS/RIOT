/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_isl29020
 * @{
 *
 * @file
 * @brief       L3G4200D adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "l3g4200d.h"

static int read(void *dev, phydat_t *res)
{
    l3g4200d_t *d = (l3g4200d_t *)dev;
    l3g4200d_read(d, (l3g4200d_data_t *)res);
    res->unit = UNIT_DPS;
    res->scale = 0;
    return 3;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t l3g4200d_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_GYRO,
};
