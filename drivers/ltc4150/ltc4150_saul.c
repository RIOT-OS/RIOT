/*
 * Copyright 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ltc4150
 * @{
 *
 * @file
 * @brief       SAUL adaption for LTC4150 devices
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
#include "ltc4150.h"

static int read_total_charge(const void *_dev, phydat_t *res)
{
    ltc4150_dev_t *dev = (ltc4150_dev_t *)_dev;
    int32_t temp[3];

    if (ltc4150_total_charge(dev, &temp[1], &temp[2]) == 0) {
        res->scale = -3;
        res->unit = UNIT_COULOMB;
#ifdef MODULE_LTC4150_BIDIRECTIONAL
        temp[0] = temp[2] - temp[1];
        int dim = (dev->params.polarity != GPIO_UNDEF) ? 3 : 1;
        phydat_fit(res, temp, (unsigned int)dim);
        return dim;
#else
        phydat_fit(res, &temp[2], 1);
        return 1;
#endif
    }

    return -ECANCELED;
}

static int read_last_minute_charge(const void *_dev, phydat_t *res)
{
    ltc4150_dev_t *dev = (ltc4150_dev_t *)_dev;
    int32_t temp[3];

    if (ltc4150_last_minute_charge(dev, &temp[1], &temp[2]) == 0) {
        res->unit = UNIT_COULOMB;
        res->scale = -3;
#ifdef MODULE_LTC4150_BIDIRECTIONAL
        temp[0] = temp[2] - temp[1];
        int dim = (dev->params.polarity != GPIO_UNDEF) ? 3 : 1;
        phydat_fit(res, temp, (unsigned int)dim);
        return dim;
#else
        phydat_fit(res, &temp[2], 1);
        return 1;
#endif
    }

    return -ECANCELED;
}

static int read_current(const void *dev, phydat_t *res)
{
    if (ltc4150_avg_current((ltc4150_dev_t *)dev, res->val) == 0) {
        res->unit = UNIT_A;
        res->scale = -4;
        return 1;
    }

    return -ECANCELED;
}

const saul_driver_t ltc4150_saul_total_charge_driver = {
    .read = read_total_charge,
    .write = saul_notsup,
    .type = SAUL_SENSE_CHARGE
};

const saul_driver_t ltc4150_saul_last_minute_charge_driver = {
    .read = read_last_minute_charge,
    .write = saul_notsup,
    .type = SAUL_SENSE_CHARGE
};

const saul_driver_t ltc4150_saul_current_driver = {
    .read = read_current,
    .write = saul_notsup,
    .type = SAUL_SENSE_CURRENT
};
