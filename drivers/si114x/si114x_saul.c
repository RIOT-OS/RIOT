/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_si114x
 * @{
 *
 * @file
 * @brief       SAUL adaption for Si114x devices family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *              Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "saul.h"
#include "si114x.h"
#include "xtimer.h"

static int read_uv(const void *dev, phydat_t *res)
{
    si114x_t *d = (si114x_t *)dev;

    res->val[0] = si114x_read_uv(d);
    res->unit = UNIT_NONE;
    res->scale = 0;
    return 1;
}

static int read_ir(const void *dev, phydat_t *res)
{
    si114x_t *d = (si114x_t *)dev;

    res->val[0] = si114x_read_ir(d);
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

static int read_visible(const void *dev, phydat_t *res)
{
    si114x_t *d = (si114x_t *)dev;

    res->val[0] = si114x_read_visible(d);
    res->unit = UNIT_LUX;
    res->scale = 0;
    return 1;
}

static int read_distance(const void *dev, phydat_t *res)
{
    si114x_t *d = (si114x_t *)dev;

    res->val[0] = si114x_read_distance(d);
    res->unit = UNIT_NONE;
    res->scale = 0;
    return 1;
}

const saul_driver_t si114x_uv_saul_driver = {
    .read = read_uv,
    .write = saul_notsup,
    .type = SAUL_SENSE_UV
};

const saul_driver_t si114x_ir_saul_driver = {
    .read = read_ir,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};

const saul_driver_t si114x_visible_saul_driver = {
    .read = read_visible,
    .write = saul_notsup,
    .type = SAUL_SENSE_LIGHT
};

const saul_driver_t si114x_distance_saul_driver = {
    .read = read_distance,
    .write = saul_notsup,
    .type = SAUL_SENSE_DISTANCE
};
