/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ccs811
 * @brief       SAUL adaption for AMS CCS811 digital gas sensor devices
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "phydat.h"
#include "saul.h"

#include "ccs811.h"

static uint16_t _iaq_tvoc = 0;
static uint16_t _iaq_eco2 = 0;

static int read(const ccs811_t *dev)
{
    /* check whether new data can be read */
    int res = ccs811_data_ready((ccs811_t *)dev);

    if (res != CCS811_OK) {
        return res;
    }

    /* read new data and save them to local storage */
    return ccs811_read_iaq((ccs811_t *)dev, &_iaq_tvoc, &_iaq_eco2, 0, 0);
}

static int read_tvoc(const void *dev, phydat_t *res)
{
    /* read new data if available */
    read(dev);

    /* fill data from local storage */
    res->val[0] = _iaq_tvoc;
    res->unit = UNIT_PPB;
    res->scale = 0;

    return 1;
}

static int read_eco2(const void *dev, phydat_t *res)
{
    /* read new data if available */
    read(dev);

    /* fill data from local storage */
    res->val[0] = _iaq_eco2;
    res->unit = UNIT_PPM;
    res->scale = 0;

    return 1;
}

const saul_driver_t ccs811_saul_driver_eco2 = {
    .read = read_eco2,
    .write = saul_notsup,
    .type = SAUL_SENSE_CO2
};

const saul_driver_t ccs811_saul_driver_tvoc = {
    .read = read_tvoc,
    .write = saul_notsup,
    .type = SAUL_SENSE_TVOC
};
