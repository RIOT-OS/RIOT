/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    mma7660_read((const mma7660_t *)dev, (mma7660_data_t *)res);

    res->unit = UNIT_G;
    res->scale = -3;

    return 3;
}

const saul_driver_t mma7660_saul_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};
