/*
 * SPDX-FileCopyrightText: 2017 Thomas Stilwell <stilwellt@openlabs.co>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_openlabs-kw41z-mini
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author    Thomas Stilwell <stilwellt@openlabs.co>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "A0",
        .line = ADC_LINE(0),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "A1",
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
    },
        {
        .name = "A2",
        .line = ADC_LINE(2),
        .res  = ADC_RES_16BIT,
    },
        {
        .name = "A3",
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "coretemp",
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "corebandgap",
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "dcdcvbat",
        .line = ADC_LINE(6),
        .res  = ADC_RES_16BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
