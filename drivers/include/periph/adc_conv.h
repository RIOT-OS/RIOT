/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Conversion routines for CPU ADC samples
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "periph/adc.h"
#if MODULE_SAUL_ADC
#include "saul/periph.h"
#endif

#ifndef BOARD_ADC_VREFH_DEFAULT
/**
 * @brief Boot default for vrefh
 */
/* 3300 mV is as good as any other guess */
#define BOARD_ADC_VREFH_DEFAULT 3300
#endif

#ifndef BOARD_ADC_VREFL_DEFAULT
/**
 * @brief Boot default for vrefl
 */
/* 0 mV is as good as any other guess */
#define BOARD_ADC_VREFL_DEFAULT 0
#endif

#ifndef BOARD_ADC_VREF_SCALE_DEFAULT
/**
 * @brief Boot default for vref scale
 */
/* millivolts by default */
#define BOARD_ADC_VREF_SCALE_DEFAULT -3
#endif

/**
 * @brief Convert ADC sample to actual voltage
 *
 * This function uses the stored ADC reference voltage for the conversion
 *
 * @param[in] sample  an ADC reading
 * @param[in] res     ADC resolution setting
 */
int adc_conv_volts(int sample, adc_res_t res);

/**
 * @brief Update the internal upper voltage reference value
 *
 * This should be called if the software knows that the reference voltage for
 * the ADC has changed since last update.
 *
 * @param[in] new_vrefh  new reference voltage
 */
void adc_conv_set_vrefh(int new_vrefh);

/**
 * @brief Update the internal lower voltage reference value
 *
 * This should be called if the software knows that the reference voltage for
 * the ADC has changed since last update.
 *
 * @param[in] new_vrefl  new reference voltage
 */
void adc_conv_set_vrefl(int new_vrefl);

/**
 * @brief Set the ADC VREF unit scale exponent
 *
 * This is the 10 exponent for the value in vrefh/vrefl, i.e.
 * Real Vrefh = vrefh * 10^(scale)
 * For example, scale = -3 means that vrefh, vrefl are given in millivolts.
 *
 * @param[in]   new_scale   Vref unit scale exponent
 */
void adc_conv_set_scale(int new_scale);

#if MODULE_SAUL_ADC || DOXYGEN
/**
 * @brief SAUL ADC compatible conversion wrapper function
 *
 * @param[in] params  SAUL ADC params pointer
 * @param[in] res     ADC sample
 *
 * @return 1
 */
int adc_conv_saul_adc_volts(const saul_adc_params_t *params, phydat_t *res);
#endif
