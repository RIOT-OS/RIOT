/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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

static bool _data_ready = false;

static int read(const ccs811_t *dev, uint16_t *iaq_tvoc, uint16_t *iaq_eco2)
{
    if (!_data_ready) {
        /* if no data were ready yet, test for new data */
        if (ccs811_data_ready(dev) == CCS811_OK) {
            _data_ready = true;
        }
        else {
            return -ECANCELED;
        }
    }

    int res = ccs811_read_iaq(dev, iaq_tvoc, iaq_eco2, NULL, NULL);

    /* in case of CCS811_ERROR_NO_NEW_DATA last valid data are returned */
    return (res == CCS811_OK ||
            res == -CCS811_ERROR_NO_NEW_DATA) ? 0 : -ECANCELED;
}

static int read_tvoc(const void *dev, phydat_t *res)
{
    if (read(dev, (uint16_t*)&res->val[0], NULL) != 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_PPB;
    res->scale = 0;
    return 1;
}

static int read_eco2(const void *dev, phydat_t *res)
{
    if (read(dev, NULL, (uint16_t*)&res->val[0]) != 0) {
        return -ECANCELED;
    }
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

const saul_driver_t ccs811_saul_driver_eco2 = {
    .read = read_eco2,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CO2
};

const saul_driver_t ccs811_saul_driver_tvoc = {
    .read = read_tvoc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TVOC
};
