/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tcs37727
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

static int read(const void *dev, phydat_t *res)
{
    tcs37727_data_t val;

    tcs37727_read((const tcs37727_t *)dev, &val);

    res->val[0] = (int16_t)val.red;
    res->val[1] = (int16_t)val.green;
    res->val[2] = (int16_t)val.blue;
    res->unit = UNIT_CD;
    res->scale = 0;

    return 3;
}

const saul_driver_t tcs37727_saul_driver = {
    .read = read,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_COLOR,
};
