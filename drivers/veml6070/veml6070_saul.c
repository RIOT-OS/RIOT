/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_veml6070
 * @{
 *
 * @file
 * @brief       SAUL adaption for VEML6070 UV sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "veml6070.h"

static int read_uv(const void *dev, phydat_t *res)
{
    res->val[0] = veml6070_read_uv((const veml6070_t *)dev);
    res->unit = UNIT_NONE;
    res->scale = -1;
    return 1;
}

const saul_driver_t veml6070_uv_saul_driver = {
    .read = read_uv,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_UV
};
