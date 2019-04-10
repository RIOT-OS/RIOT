/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_firefly
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"
#include "periph/adc.h"
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
        .line = ADC_LINE(0),
        .res  = ADC_RES_12BIT,
    },
    {
        .line = ADC_LINE(1),
        .res  = ADC_RES_12BIT,
    },
    {
        .line = ADC_LINE(2),
        .res  = ADC_RES_12BIT,
    }
};

/**
 * @brief   ADC information for SAUL registry
 */
static const saul_reg_info_t saul_adc_info[] =
{
    { .name = "ADC1" },
    { .name = "ADC2" },
    { .name = "ADC3" }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */
