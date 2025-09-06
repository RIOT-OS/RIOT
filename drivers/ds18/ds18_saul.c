/*
 * SPDX-FileCopyrightText: 2017 Frits Kuipers
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ds18
 * @{
 *
 * @file
 * @brief       SAUL adaption for Maxim Integrated DS1822 and DS18B20 temperature sensors
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <string.h>

#include "saul.h"
#include "ds18.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    if (ds18_get_temperature(dev, &res->val[0]) == DS18_ERROR) {
        return -ECANCELED;
    }

    res->unit = UNIT_TEMP_C;
    res->scale = -2;
    return 1;
}

const saul_driver_t ds18_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP
};
