/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_l3g4200d
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

static int read(const void *dev, phydat_t *res)
{
    l3g4200d_read((const l3g4200d_t *)dev, (l3g4200d_data_t *)res);
    res->unit = UNIT_DPS;
    res->scale = 0;
    return 3;
}

const saul_driver_t l3g4200d_saul_driver = {
    .read = read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_GYRO,
};
