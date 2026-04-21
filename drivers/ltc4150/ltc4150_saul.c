/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

static int read_charge(const void *_dev, phydat_t *res)
{
    ltc4150_dev_t *dev = (ltc4150_dev_t *)_dev;
    int32_t temp[3];

    if (ltc4150_charge(dev, (uint32_t *)&temp[1], (uint32_t *)&temp[2]) == 0) {
        res->scale = -3;
        res->unit = UNIT_COULOMB;
        temp[0] = temp[2] - temp[1];
        int dim = (gpio_is_valid(dev->params.polarity)) ? 3 : 1;
        phydat_fit(res, temp, (unsigned)dim);
        return dim;
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

const saul_driver_t ltc4150_saul_charge_driver = {
    .read = read_charge,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CHARGE
};

const saul_driver_t ltc4150_saul_current_driver = {
    .read = read_current,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CURRENT
};
