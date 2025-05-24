/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   boards_mulle
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
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
        .name = "k60temp",
        .line = ADC_LINE(0),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "k60vrefsh",
        .line = ADC_LINE(2),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "k60vrefsl",
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "k60bandgap",
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "DAC0feedback",
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "VREFfeedback",
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "Vbat",
        .line = MULLE_VBAT_ADC_LINE,
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "Vchr",
        .line = MULLE_VCHR_ADC_LINE,
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PGA0_DP",
        .line = ADC_LINE(8),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PGA0_DM",
        .line = ADC_LINE(9),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTA17",
        .line = ADC_LINE(10),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTB0",
        .line = ADC_LINE(11),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTC0",
        .line = ADC_LINE(12),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTC8",
        .line = ADC_LINE(13),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTC9",
        .line = ADC_LINE(14),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTC10",
        .line = ADC_LINE(15),
        .res  = ADC_RES_16BIT,
    },
    {
        .name = "PTC11",
        .line = ADC_LINE(16),
        .res  = ADC_RES_16BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
