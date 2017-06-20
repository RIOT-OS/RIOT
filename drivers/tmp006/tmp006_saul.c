/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_tmp006
 * @{
 *
 * @file
 * @brief       TMP006 adaption to the RIOT actuator/sensor interface
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "tmp006.h"

static int read_temp(void *dev, phydat_t *res)
{
    if (tmp006_read_temperature((tmp006_t *)dev, &res->val[0], &res->val[1]) != TMP006_OK) {
        return -ECANCELED;
    }
    res->val[2] = 0;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 2;
}

const saul_driver_t tmp006_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};
