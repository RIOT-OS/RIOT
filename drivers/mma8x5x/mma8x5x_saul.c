/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mma8x5x
 * @{
 *
 * @file
 * @brief       MMA8x5x adaption to the RIOT actuator/sensor interface
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "mma8x5x.h"

static int read_acc(const void *dev, phydat_t *res)
{
    mma8x5x_read((const mma8x5x_t *)dev, (mma8x5x_data_t *)res->val);

    res->unit = UNIT_G_FORCE;
    res->scale = -3;

    return 3;
}

const saul_driver_t mma8x5x_saul_driver = {
    .read = read_acc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ACCEL,
};
