/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_si1133
 * @{
 *
 * @file
 * @brief       SAUL adaption for SI1133 devices
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "saul.h"
#include "si1133.h"

static int read_uv(const void *dev, phydat_t *res)
{
    if (si1133_easy_configure((si1133_t *)dev,
                              SI1133_SENS_UV | SI1133_SENS_DEEP_UV,
                              /*sample_time_log=*/ 1,
                              /*sw_gain=*/ 0) != 0) {
        return -ECANCELED;
    }

    int32_t values[2];
    if (si1133_capture_sensors((si1133_t *)dev, values,
                               ARRAY_SIZE(values)) != 0) {
        return -ECANCELED;
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(values); i++) {
        res->val[i] = values[i];
    }
    res->unit = UNIT_NONE;  /* UV index */
    res->scale = 0;
    return ARRAY_SIZE(values);
}

static int read_ir(const void *dev, phydat_t *res)
{
    if (si1133_easy_configure((si1133_t *)dev,
                              SI1133_SENS_SMALL_IR | SI1133_SENS_MEDIUM_IR |
                              SI1133_SENS_LARGE_IR,
                              /*sample_time_log=*/ 1,
                              /*sw_gain=*/ 0) != 0) {
        return -ECANCELED;
    }

    int32_t values[3];
    if (si1133_capture_sensors((si1133_t *)dev, values,
                               ARRAY_SIZE(values)) != 0) {
        return -ECANCELED;
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(values); i++) {
        res->val[i] = values[i];
    }
    res->unit = UNIT_LUX;
    res->scale = 0;
    return ARRAY_SIZE(values);
}

static int read_white(const void *dev, phydat_t *res)
{
    if (si1133_easy_configure((si1133_t *)dev,
                              SI1133_SENS_WHITE | SI1133_SENS_LARGE_WHITE,
                              /*sample_time_log=*/ 1,
                              /*sw_gain=*/ 0) != 0) {
        return -ECANCELED;
    }

    int32_t values[2];
    if (si1133_capture_sensors((si1133_t *)dev, values,
                               ARRAY_SIZE(values)) != 0) {
        return -ECANCELED;
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(values); i++) {
        res->val[i] = values[i];
    }
    res->unit = UNIT_LUX;
    res->scale = 0;
    return ARRAY_SIZE(values);
}

const saul_driver_t si1133_uv_saul_driver = {
    .read = read_uv,
    .write = saul_notsup,
    .type = SAUL_SENSE_UV
};

const saul_driver_t si1133_ir_saul_driver = {
    .read = read_ir,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};

const saul_driver_t si1133_visible_saul_driver = {
    .read = read_white,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};
