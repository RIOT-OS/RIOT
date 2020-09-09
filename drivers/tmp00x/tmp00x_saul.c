/*
 * Copyright (C) 2017 - 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tmp00x
 * @{
 *
 * @file
 * @brief       TMP00X (TMP006 and TMP007) adaption to the RIOT actuator/sensor interface
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "tmp00x.h"
#include "kernel_defines.h"

static int read_temp(const void *dev, phydat_t *res)
{
    if (tmp00x_read_temperature((const tmp00x_t *)dev, &res->val[0],
                                &res->val[1]) != TMP00X_OK) {
        return -ECANCELED;
    }
    res->val[2] = 0;
    if (IS_ACTIVE(CONFIG_TMP00X_USE_RAW_VALUES)) {
        res->unit = UNIT_NONE;
        res->scale = 0;
    }
    else {
        res->unit = UNIT_TEMP_C;
        res->scale = -2;
    }
    return 2;
}

const saul_driver_t tmp00x_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_OBJTEMP,
};
