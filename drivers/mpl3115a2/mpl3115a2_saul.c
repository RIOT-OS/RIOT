/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mpl3115a2
 * @{
 *
 * @file
 * @brief       MPL3115A2 adaption to the RIOT actuator/sensor interface
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "log.h"
#include "saul.h"
#include "mpl3115a2.h"

static int read_pressure(const void *dev, phydat_t *res)
{
    uint8_t state;
    uint32_t pressure;
    if (mpl3115a2_read_pressure((const mpl3115a2_t *)dev,
                                &pressure, &state) != MPL3115A2_OK) {
        LOG_ERROR("[SAUL] mpl3115a2_read_pressure failed!\n");
        return -1;
    }
    /* pressure values > 100000, so /100 for int16_t which matches unit hPA */
    res->val[0] = (int16_t)(pressure/100);
    res->unit = UNIT_PA;
    res->scale = 2;

    return 1;
}

static int read_temperature(const void *dev, phydat_t *res)
{
    int16_t temperature;
    if (mpl3115a2_read_temp((const mpl3115a2_t *)dev, &temperature) != MPL3115A2_OK) {
        LOG_ERROR("[SAUL] mpl3115a2_read_temp failed!\n");
        return -1;
    }

    res->val[0] = temperature;
    res->unit = UNIT_TEMP_C;
    res->scale = -1;

    return 1;
}

const saul_driver_t mpl3115a2_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t mpl3115a2_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};
