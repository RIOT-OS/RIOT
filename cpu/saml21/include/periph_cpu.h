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
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (2)
/** @} */

/**
 * @name   SAML21 GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN  = 0,                 /**< Main clock */
    SAM0_GCLK_8MHZ  = 1,                 /**< 8MHz clock */
    SAM0_GCLK_32KHZ = 2,                 /**< 32 kHz clock */
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
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   The MCU has a 12 bit DAC
 */
#define DAC_RES_BITS        (12)

/**
 * @brief   The MCU has two DAC outputs.
 */
#define DAC_NUMOF           (2)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
