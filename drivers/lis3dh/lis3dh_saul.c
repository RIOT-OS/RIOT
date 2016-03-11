/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   driver_lis3dh
 * @{
 *
 * @file
 * @brief     LIS3DH adaption to the RIOT actuator/sensor interface
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "lis3dh.h"

static int read_acc(void *dev, phydat_t *res)
{
    lis3dh_data_t xyz;

    lis3dh_t *d = (lis3dh_t *)dev;
    int err = lis3dh_read_xyz(d, &xyz);
    if (err != 0) {
        /* Something went wrong in the LIS3DH driver */
        return -ECANCELED;
    }

    res->val[0] = xyz.acc_x;
    res->val[1] = xyz.acc_y;
    res->val[2] = xyz.acc_z;
    /* unit: milli-G */
    res->scale = -3;
    res->unit = UNIT_G;

    return 3;
}

static int write(void *dev, phydat_t *state)
{
    (void) dev;
    (void) state;
    return -ENOTSUP;
}

const saul_driver_t lis3dh_saul_driver = {
    .read = read_acc,
    .write = write,
    .type = SAUL_SENSE_ACCEL,
};
