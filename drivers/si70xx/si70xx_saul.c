/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si70xx
 * @{
 *
 * @file
 * @brief       SAUL adoption for Si7006/13/20/21 sensor.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "saul.h"

#include "si70xx.h"

static int read_temperature(const void *dev, const uint8_t ctxt, phydat_t *res)
{
    (void)ctxt;

    res->val[0] = (int32_t) si70xx_get_temperature((const si70xx_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_relative_humidity(const void *dev, const uint8_t ctxt, phydat_t *res)
{
    (void)ctxt;

    res->val[0] = (int32_t) si70xx_get_relative_humidity((const si70xx_t *)dev);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t si70xx_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t si70xx_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM
};
