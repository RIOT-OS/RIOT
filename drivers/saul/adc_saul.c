/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#if MODULE_ANALOG_UTIL
#include "analog_util.h"
#endif

/* Unscaled version of sampling driver */
static int read_adc(const void *dev, phydat_t *res)
{
    const saul_adc_params_t *params = (const saul_adc_params_t *)dev;
    res->val[0] = adc_sample(params->line, params->res);
    memset(&(res->val[1]), 0, 2 * sizeof(res->val[1]));
    /* Raw ADC reading has no unit */
    res->unit = UNIT_NONE;
    res->scale = 0;
    return 1;
}

const saul_driver_t adc_saul_driver = {
    .read = read_adc,
    .write = saul_notsup,
    .type = SAUL_SENSE_ANALOG,
};

/* Scaling version of sampling driver */
#if MODULE_ANALOG_UTIL
static int read_adc_scaled(const void *dev, phydat_t *res)
{
    const saul_adc_params_t *params = (const saul_adc_params_t *)dev;
    int raw = adc_sample(params->line, params->res);
    int32_t value = adc_util_map(raw, params->res, params->val_min, params->val_max);
    res->val[0] = value;
    memset(&(res->val[1]), 0, 2 * sizeof(res->val[1]));
    res->unit = params->unit;
    res->scale = params->scale;
    return 1;
}

/**
 * @brief   SAUL driver template
 */
#define ADC_SAUL_DRIVER_INSTANCE(saul_type, name) \
    const saul_driver_t adc_saul_ ## name ## _driver = { \
    .read = read_adc_scaled, \
    .write = saul_notsup, \
    .type = saul_type, \
}

ADC_SAUL_DRIVER_INSTANCE(SAUL_SENSE_TEMP, temp);
ADC_SAUL_DRIVER_INSTANCE(SAUL_SENSE_ANALOG, analog);
#endif
