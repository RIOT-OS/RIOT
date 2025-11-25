/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
