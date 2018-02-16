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
#if MODULE_SAUL_ADC_CONVERSION
#include "periph/adc_conv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_SAUL_ADC_CONVERSION
#define ADC_CONV(func) .conv = (func)
#else
#define ADC_CONV(ignore)
#endif

#if MODULE_SAUL_ADC_CONVERSION
/**
 * @brief Convert Mulle Vbat, Vchr samples into real voltage
 *
 * @param[in] params SAUL ADC params pointer
 * @param[in] res    ADC sample
 *
 * @return 1
 */
static int mulle_vbat_volts(const saul_adc_params_t *params, phydat_t *res)
{
    adc_conv_saul_adc_volts(params, res);
    /* The Vbat, Vchr measurement point is located on a voltage divider which
     * shows half of the full voltage, to be able to measure voltages greater
     * than ADC Vref. */
    res->val[0] *= 2;
    return 1;
}
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
        ADC_CONV(adc_conv_saul_adc_volts),
    },
    {
        .name = "k60vrefsl",
        .line = ADC_LINE(3),
        .res  = ADC_RES_16BIT,
        ADC_CONV(adc_conv_saul_adc_volts),
    },
    {
        .name = "k60bandgap",
        .line = ADC_LINE(1),
        .res  = ADC_RES_16BIT,
        ADC_CONV(adc_conv_saul_adc_volts),
    },
    {
        .name = "DAC0feedback",
        .line = ADC_LINE(4),
        .res  = ADC_RES_16BIT,
        ADC_CONV(adc_conv_saul_adc_volts),
    },
    {
        .name = "VREFfeedback",
        .line = ADC_LINE(5),
        .res  = ADC_RES_16BIT,
        ADC_CONV(adc_conv_saul_adc_volts),
    },
    {
        .name = "Vbat",
        .line = MULLE_VBAT_ADC_LINE,
        .res  = ADC_RES_16BIT,
        ADC_CONV(mulle_vbat_volts),
    },
    {
        .name = "Vchr",
        .line = MULLE_VCHR_ADC_LINE,
        .res  = ADC_RES_16BIT,
        ADC_CONV(mulle_vbat_volts),
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

#endif /* ADC_PARAMS_H */
/** @} */
