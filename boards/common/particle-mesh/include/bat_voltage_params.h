/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_particle-mesh
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped battery voltage information
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 */

#ifndef BAT_VOLTAGE_PARAMS_H
#define BAT_VOLTAGE_PARAMS_H

#include "saul/bat_voltage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Conversion function to convert ADC sample to battery voltage
 *
 * @param[in] adc_sample    The raw ADC sample.
 *
 * @return Voltage value for phydat.
 */
int16_t saul_bat_voltage_convert(int32_t adc_sample);

/**
 * @brief   Battery voltage configuration
 */
static const saul_bat_voltage_params_t saul_bat_voltage_params[] =
{
    {
        .name = "BAT",
        .phydat_scale = -3,
        .line = ADC_LINE(3),
        .res = ADC_RES_10BIT,
        .convert = saul_bat_voltage_convert,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* BAT_VOLTAGE_PARAMS_H */
/** @} */
