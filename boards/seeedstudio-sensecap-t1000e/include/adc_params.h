/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-sensecap-t1000e
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped ADC lines
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "periph/adc.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ADC SAUL configuration
 *
 * @note    NTC_THERMISTOR exposes a raw ADC reading, not degrees Celsius.
 *          NTC curve conversion must be done at the application level.
 */
static const saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "LIGHT",
        .line = ADC_LINE(NRF52_AIN5),   /* P0.29 */
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "NTC_THERMISTOR",
        .line = ADC_LINE(NRF52_AIN7),   /* P0.31 */
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "BAT",
        .line = ADC_LINE(NRF52_AIN0),   /* P0.02 */
        .res  = ADC_RES_10BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
