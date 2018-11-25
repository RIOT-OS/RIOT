/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp9808
 * @{
 *
 * @file
 * @brief       SAUL adaption for MCP9808 device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "mcp9808.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    if (mcp9808_read_temperature((const mcp9808_t *)dev, &res->val[0]) == MCP9808_OK) {
        res->unit = UNIT_TEMP_C;
        res->scale = -1;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t mcp9808_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};
