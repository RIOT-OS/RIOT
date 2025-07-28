/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-FileCopyrightText: 2020 Oppila Microsystems - http://www.oppila.in
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_omote
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC in omote
 *
 */

#include "board.h"
#include "saul/periph.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "ADC1",
        .line = ADC_LINE(0),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "ADC2",
        .line = ADC_LINE(1),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "ADC3",
        .line = ADC_LINE(2),
        .res  = ADC_RES_12BIT,
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
