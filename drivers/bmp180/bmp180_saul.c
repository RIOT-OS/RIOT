/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmp180
 * @{
 *
 * @file
 * @brief       SAUL adaption for BMP180 device
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "bmp180.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    res->val[0] = bmp180_read_temperature((const bmp180_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = -1;
    return 1;
}

static int read_pressure(const void *dev, phydat_t *res)
{
    res->val[0] = bmp180_read_pressure((const bmp180_t *)dev) / 100;
    res->unit = UNIT_PA;
    res->scale = 2;
    return 1;
}

const saul_driver_t bmp180_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t bmp180_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS
};
