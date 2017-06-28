/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lps25h
 * @{
 *
 * @file
 * @brief       SAUL adaption for LPS25H device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "lps25h.h"
#include "xtimer.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    res->val[0] = lps25h_read_temperature((const lps25h_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = 0;
    return 1;
}

static int read_pressure(const void *dev, phydat_t *res)
{
    res->val[0] = lps25h_read_pressure((const lps25h_t *)dev);
    res->unit = UNIT_BAR;
    res->scale = -3;
    return 1;
}

const saul_driver_t lps25h_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

const saul_driver_t lps25h_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS
};
