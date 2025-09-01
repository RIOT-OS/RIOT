/*
 * SPDX-FileCopyrightText: 2020 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_mcb2388
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Benjamin Valentin <benpicco@beuth-hochschule.de>
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
        .name = "POT1",
        .line = ADC_LINE(0),
        .res  = ADC_RES_10BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
