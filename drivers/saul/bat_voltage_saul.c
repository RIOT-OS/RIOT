/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL wrapper to gauge battery voltage.
 *
 * Adafruit Feather-type boards typically have an ADC pin exposed to read
 * the battery voltage.
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "saul/bat_voltage.h"
#include "phydat.h"
#include "periph/adc.h"

static int read_adc(const void *dev, phydat_t *res)
{
    const saul_bat_voltage_params_t *params = *((const saul_bat_voltage_params_t **)dev);
    int32_t sample = adc_sample(params->line, params->res);
    res->val[0] = params->convert(sample);
    res->unit = UNIT_V;
    res->scale = params->phydat_scale;
    return 1;
}

const saul_driver_t bat_voltage_saul_driver = {
    .read = read_adc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_VOLTAGE,
};
