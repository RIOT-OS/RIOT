/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ina2xx
 * @{
 *
 * @file
 * @brief       SAUL adaption of the INA2XX driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "ina2xx.h"

static int read_current(const void *_dev, phydat_t *res)
{
    ina2xx_t *dev = (ina2xx_t *)_dev;
    int32_t current;

    if (ina2xx_read_current(dev, &current) == 0) {
        res->scale = -5;
        res->unit = UNIT_A;
        phydat_fit(res, &current, 1);
        return 1;
    }

    return -ECANCELED;
}

static int read_power(const void *_dev, phydat_t *res)
{
    ina2xx_t *dev = (ina2xx_t *)_dev;
    uint32_t power;
    if (ina2xx_read_power(dev, &power) == 0) {
        res->unit = UNIT_W;
        res->scale = -4;
        phydat_fit(res, (int32_t *)&power, 1);
        return 1;
    }

    return -ECANCELED;
}

static int read_voltage(const void *_dev, phydat_t *res)
{
    ina2xx_t *dev = (ina2xx_t *)_dev;
    if (ina2xx_read_bus(dev, (uint16_t *)&res->val[0]) >= 0) {
        res->unit = UNIT_V;
        res->scale = -3;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t ina2xx_saul_current_driver = {
    .read = read_current,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CURRENT
};

const saul_driver_t ina2xx_saul_power_driver = {
    .read = read_power,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_POWER
};

const saul_driver_t ina2xx_saul_voltage_driver = {
    .read = read_voltage,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_VOLTAGE
};
