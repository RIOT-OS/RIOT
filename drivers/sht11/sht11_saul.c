/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht11
 * @{
 *
 * @file
 * @brief       SHT11 adaption to the RIOT actuator/sensor interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "sht11.h"

static int read_temp(const void *dev, phydat_t *res)
{
    (void)dev;
    sht11_val_t result;
    if (sht11_read_sensor(&result, TEMPERATURE) != 1) {
        return -ECANCELED;
    }
    res->val[0] = (uint16_t)(result.temperature*100.0);
    res->scale = -2;
    res->unit = UNIT_TEMP_C;

    return 1;
}

static int read_hum(const void *dev, phydat_t *res)
{
    (void)dev;
    sht11_val_t result;
    /* We want temperature corrected humidity --> reading temperature as well */
    if (sht11_read_sensor(&result, HUMIDITY|TEMPERATURE) != 1) {
        return -ECANCELED;
    }
    res->val[0] = (uint16_t)(result.relhum_temp*100.0);
    res->scale = -2;
    res->unit = UNIT_PERCENT;

    return 1;
}

const saul_driver_t sht11_temp_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t sht11_hum_saul_driver = {
    .read = read_hum,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
