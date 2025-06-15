/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tsl2561
 * @{
 *
 * @file
 * @brief       SAUL adaption for TSL2561 device
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "saul.h"
#include "tsl2561.h"

static int read_illuminance(const void *dev, phydat_t *res)
{
    res->val[0] = tsl2561_read_illuminance((const tsl2561_t *)dev);
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

const saul_driver_t tsl2561_illuminance_saul_driver = {
    .read = read_illuminance,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_LIGHT
};
