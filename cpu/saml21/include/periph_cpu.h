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
 * @brief   The Low Power SRAM is not retained during deep sleep.
 */
#define CPU_BACKUP_RAM_NOT_RETAINED (1)

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
#if defined(CPU_MODEL_SAML21E18A) || defined(CPU_MODEL_SAML21E18B) || \
    defined(CPU_MODEL_SAML21E17A) || defined(CPU_MODEL_SAML21E17B) || \
    defined(CPU_MODEL_SAML21E16A) || defined(CPU_MODEL_SAML21E16B) || \
    defined(CPU_MODEL_SAML21E15A) || defined(CPU_MODEL_SAML21E15B)
static const int8_t exti_config[1][32] = {
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, -1, -1, 14, 15,
      0,  1,  2,  3, -1, -1,  6,  7, 12, 13, -1, 15, -1, -1, 10, 11},
};
#else /* CPU_MODEL_SAML21E */
static const int8_t exti_config[2][32] = {
#if defined(CPU_MODEL_SAML21G18A) || defined(CPU_MODEL_SAML21G18B) || \
    defined(CPU_MODEL_SAML21G17A) || defined(CPU_MODEL_SAML21G17B) || \
    defined(CPU_MODEL_SAML21G16A) || defined(CPU_MODEL_SAML21G16B)
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15, -1, -1, 10, 11},
    {-1, -1,  2,  3, -1, -1, -1, -1,  8,  9, 10, 11, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1},
#elif defined(CPU_MODEL_SAML21J18A) || defined(CPU_MODEL_SAML21J18B) || \
      defined(CPU_MODEL_SAML21J17A) || defined(CPU_MODEL_SAML21J17B) || \
      defined(CPU_MODEL_SAML21J16A) || defined(CPU_MODEL_SAML21J16B)
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15, -1, -1, 10, 11},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15},
#elif defined(CPU_MODEL_SAMR30G18A) || defined(CPU_MODEL_SAMR34J18B)
    { 0,  1, -1, -1,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4, -1,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0, -1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15},
#elif defined(CPU_MODEL_SAMR30E18A)
    {-1, -1, -1, -1, -1, -1,  6,  7, -1,  9, 10, 11, -1, -1, 14, 15,
      0,  1,  2,  3,  4, -1, -1, -1, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15,
      0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14, 15},
#else
    #error Please define a proper CPU_MODEL.
#endif
};
#endif /* CPU_MODEL_SAML21E */

#ifndef DOXYGEN
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xff,                       /**< not supported */
    ADC_RES_8BIT  = ADC_CTRLC_RESSEL_8BIT,      /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLC_RESSEL_10BIT,     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLC_RESSEL_12BIT,     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xfe,                       /**< not supported */
    ADC_RES_16BIT = 0xfd                        /**< not supported */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
