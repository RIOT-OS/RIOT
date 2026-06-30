/*
 * SPDX-FileCopyrightText: 2026 Anastasiia
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup   drivers_lis331dlh
 * @{
 *
 * @file
 * @brief     LIS331DLH adaption to the RIOT actuator/sensor interface
 *
 * @}
 */

#include <errno.h>

#include "saul.h"
#include "lis331dlh.h"

static int read_acc(const void *dev, phydat_t *res)
{
    lis331dlh_data_t data;

    if (lis331dlh_read_xyz((const lis331dlh_t *)dev, &data) != 0) {
        return -ECANCELED;
    }

    res->val[0] = data.x;
    res->val[1] = data.y;
    res->val[2] = data.z;

    res->scale = -3;
    res->unit = UNIT_G_FORCE;

    return 3;
}

const saul_driver_t lis331dlh_saul_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
