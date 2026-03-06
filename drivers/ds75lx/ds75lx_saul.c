/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ds75lx
 * @{
 *
 * @file
 * @brief       SAUL adaption for DS75LX device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "ds75lx.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    ds75lx_wakeup((const ds75lx_t *)dev);
    if (ds75lx_read_temperature((const ds75lx_t *)dev, &res->val[0]) != DS75LX_OK) {
        ds75lx_shutdown((const ds75lx_t *)dev);
        return -ECANCELED;
    }
    ds75lx_shutdown((const ds75lx_t *)dev);

    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t ds75lx_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
