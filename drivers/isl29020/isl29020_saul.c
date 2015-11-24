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
 * @brief       ISL29020 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "isl29020.h"

static int read(void *dev, phydat_t *res)
{
    isl29020_t *d = (isl29020_t *)dev;
    res->val[0] = (int16_t)isl29020_read(d);
    memset(&(res->val[1]), 0, 2 * sizeof(int16_t));
    res->unit = UNIT_CD;
    res->scale = 0;
    return 1;
}

static int write(void *dev, phydat_t *state)
{
    return -ENOTSUP;
}

const saul_driver_t isl29020_saul_driver = {
    .read = read,
    .write = write,
    .type = SAUL_SENSE_LIGHT,
};
