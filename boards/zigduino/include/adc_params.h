/*
 * SPDX-FileCopyrightText: 2021 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_zigduino
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped ADC
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
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
        .name = "temp",
        .line = ADC_LINE(0),
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "light",
        .line = ADC_LINE(1),
        .res  = ADC_RES_10BIT,
    },
    {
        .name = "loud",
        .line = ADC_LINE(2),
        .res  = ADC_RES_10BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
