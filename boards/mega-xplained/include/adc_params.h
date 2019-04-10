/*
 * Copyright (C) 2016 Eistec AB
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .line = NTC_OUTPUT,
        .res  = ADC_RES_10BIT,
    },
    {
        .line = LIGHT_SENSOR_OUTPUT,
        .res  = ADC_RES_10BIT,
    },
    {
        .line = FILTER_OUTPUT,
        .res  = ADC_RES_10BIT,
    }
};

/**
 * @brief   ADC information for SAUL registry
 */
static const saul_reg_info_t saul_adc_info[] =
{
    { .name = "NTC thermistor" },
    { .name = "Light sensor" },
    { .name = "RC filter" }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */
