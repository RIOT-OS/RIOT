/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_mma8x5x
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

static int read_acc(void *dev, phydat_t *res)
{
    mma8x5x_read((mma8x5x_t *)dev, (mma8x5x_data_t *)res);

    res->unit = UNIT_G;
    res->scale = -3;

    return 3;
}

const saul_driver_t mma8x5x_saul_driver = {
    .read = read_acc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};
