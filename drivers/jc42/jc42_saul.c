/*
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_jc42
 * @{
 *
 * @file
 * @brief       SAUL adaption for jc42 compatible device
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "jc42.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    int16_t temperature;

    jc42_get_temperature((const jc42_t *)dev, &temperature);
    res->val[0] = temperature;
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t jc42_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
