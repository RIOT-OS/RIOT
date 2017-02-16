/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_tcs37727
 * @{
 *
 * @file
 * @brief       TCS37727 adaption to the RIOT actuator/sensor interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "tcs37727.h"

static int read(void *dev, phydat_t *res)
{
    tcs37727_t *d = (tcs37727_t *)dev;
    tcs37727_data_t val;

    tcs37727_read(d, &val);

    res->val[0] = (int16_t)val.red;
    res->val[1] = (int16_t)val.green;
    res->val[2] = (int16_t)val.blue;
    res->unit = UNIT_CD;
    res->scale = 0;

    return 3;
}

const saul_driver_t tcs37727_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_COLOR,
};
