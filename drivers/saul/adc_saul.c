/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL wrapper for direct access to analog pins
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "saul/periph.h"
#include "phydat.h"
#include "periph/adc.h"

static int read_adc(const void *dev, phydat_t *res)
{
    const saul_adc_params_t *params = *((const saul_adc_params_t **)dev);
    res->val[0] = adc_sample(params->line, params->res);
    /* Raw ADC reading has no unit */
    res->unit = UNIT_NONE;
    res->scale = 0;
    return 1;
}

const saul_driver_t adc_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_ANALOG,
};
