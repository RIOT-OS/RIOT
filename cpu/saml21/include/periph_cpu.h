/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml21
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[2][32] = {
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15, -1, -1, 10, 11},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15},
};

#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x0f,                       /**< not supported */
    ADC_RES_8BIT  = ADC_CTRLC_RESSEL_8BIT,      /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLC_RESSEL_10BIT,     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLC_RESSEL_12BIT,     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0x09,                       /**< not supported */
    ADC_RES_16BIT = 0x0a,                       /**< not supported */
    ADC_RES_18BIT = 0x0b,                       /**< not supported */
    ADC_RES_20BIT = 0x0c,                       /**< not supported */
    ADC_RES_22BIT = 0x0d,                       /**< not supported */
    ADC_RES_24BIT = 0x0e,                       /**< not supported */
} adc_res_t;
/** @} */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
