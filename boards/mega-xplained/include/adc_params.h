/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-FileCopyrightText: 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_mega-xplained
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author    Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "NTC thermistor",
        .line = NTC_OUTPUT,
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "Light sensor",
        .line = LIGHT_SENSOR_OUTPUT,
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "RC filter",
        .line = FILTER_OUTPUT,
        .res  = ADC_RES_10BIT,
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
