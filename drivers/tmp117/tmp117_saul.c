/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tmp117
 * @{
 *
 * @file
 * @brief       TMP117 adaption to the RIOT actuator/sensor interface
 *
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "saul.h"
#include "tmp117.h"

static int _read_temp(const void *dev, phydat_t *data)
{


    memset(data, 0, sizeof(phydat_t));
    if (tmp117_read_temperature((tmp117_t *)dev, &data->val[0]) != TMP117_OK) {
        return -ECANCELED;
    }
    data->unit = UNIT_TEMP_C;
    data->scale = -2; /* centi-degrees */

    return 1;
}

const saul_driver_t tmp117_saul_driver = {
    .read = _read_temp,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
