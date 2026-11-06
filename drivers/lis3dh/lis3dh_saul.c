/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup   drivers_lis3dh
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

static int read_acc(const void *dev, phydat_t *res)
{
    lis3dh_data_t xyz;

    int err = lis3dh_read_xyz((const lis3dh_t *)dev, &xyz);
    if (err != 0) {
        /* Something went wrong in the LIS3DH driver */
        return -ECANCELED;
    }

    res->val[0] = xyz.acc_x;
    res->val[1] = xyz.acc_y;
    res->val[2] = xyz.acc_z;
    /* unit: milli-G */
    res->scale = -3;
    res->unit = UNIT_G_FORCE;

    return 3;
}

const saul_driver_t lis3dh_saul_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
