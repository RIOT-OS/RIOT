/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mma7660
 * @{
 *
 * @file
 * @brief       MMA7660 adaption to the RIOT actuator/sensor interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "mma7660.h"

static int read_acc(const void *dev, phydat_t *res)
{
    mma7660_read((const mma7660_t *)dev, (mma7660_data_t *)res->val);

    res->unit = UNIT_G_FORCE;
    res->scale = -3;

    return 3;
}

const saul_driver_t mma7660_saul_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
