/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_isl29020
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

static int read(const void *dev, phydat_t *res)
{
    res->val[0] = (int16_t)isl29020_read((const isl29020_t *)dev);
    res->val[1] = 0;
    res->val[2] = 0;
    res->unit = UNIT_CD;
    res->scale = 0;
    return 1;
}

const saul_driver_t isl29020_saul_driver = {
    .read = read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT,
};
