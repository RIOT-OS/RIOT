/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_adcxx1c
 * @{
 *
 * @file
 * @brief       ADCxx1C adaption to the RIOT actuator/sensor interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>

#include "saul.h"
#include "adcxx1c.h"

static int read_adc(const void *dev, phydat_t *res)
{
    if (adcxx1c_read_raw((const adcxx1c_t *)dev, &res->val[0])) {
        return -ECANCELED;
    }

    res->unit = UNIT_NONE;
    res->scale = 0;

    return 1;
}

const saul_driver_t adcxx1c_saul_driver = {
    .read = read_adc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ANALOG,
};
