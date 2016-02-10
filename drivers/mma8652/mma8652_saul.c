/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_mma8652
 * @{
 *
 * @file
 * @brief       MMA8652 adaption to the RIOT actuator/sensor interface
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "mma8652.h"

static int read_acc(void *dev, phydat_t *res)
{
    int16_t x, y, z;
    uint8_t status;

    mma8652_t *d = (mma8652_t *)dev;
    mma8652_read(d, &x, &y, &z, &status);

    res->val[0] = x;
    res->val[1] = y;
    res->val[2] = z;
    res->unit = UNIT_G;
    res->scale = -3;

    return 3;
}

static int write(void *dev, phydat_t *state)
{
    (void) dev;
    (void) state;
    return -ENOTSUP;
}

const saul_driver_t mma8652_saul_driver = {
    .read = read_acc,
    .write = write,
    .type = SAUL_SENSE_ACCEL,
};
