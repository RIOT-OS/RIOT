/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_mulle
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
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
        .line = ADC_LINE(0),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(2),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = MULLE_VBAT_ADC_LINE,
        .res  = ADC_RES_16BIT,
    },
    {
        .line = MULLE_VCHR_ADC_LINE,
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(8),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(9),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(10),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(11),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(12),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(13),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(14),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(15),
        .res  = ADC_RES_16BIT,
    },
    {
        .line = ADC_LINE(16),
        .res  = ADC_RES_16BIT,
    },
};

/**
 * @brief   ADC information for SAUL registry
 */
static const saul_reg_info_t saul_adc_info[] =
{
    { .name = "k60temp" },
    { .name = "k60vrefsh" },
    { .name = "k60vrefsl" },
    { .name = "k60bandgap" },
    { .name = "DAC0feedback" },
    { .name = "VREFfeedback" },
    { .name = "Vbat" },
    { .name = "Vchr" },
    { .name = "PGA0_DP" },
    { .name = "PGA0_DM" },
    { .name = "PTA17" },
    { .name = "PTB0" },
    { .name = "PTC0" },
    { .name = "PTC8" },
    { .name = "PTC9" },
    { .name = "PTC10" },
    { .name = "PTC11" }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */
