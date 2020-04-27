/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml1x
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (1)
/** @} */

/**
 * @brief   Override the default initial PM blocker
 * @todo   Idle modes are enabled by default, deep sleep mode blocked
 */
#define PM_BLOCKER_INITIAL  0x00000001

/**
 * @name   SAML1x GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN = 0,                 /**< 16 MHz main clock      */
    SAM0_GCLK_32KHZ,                    /**< 32 kHz clock           */
};
/** @} */

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
#endif /* ndef DOXYGEN */
/** @} */

/**
 * @brief   The MCU has a 10 bit DAC
 */
#define DAC_RES_BITS        (10)

/**
 * @brief   The MCU has one DAC Output.
 */
#define DAC_NUMOF           (1)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
