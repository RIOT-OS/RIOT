/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f4
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_F4_PERIPH_CPU_H
#define PERIPH_F4_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1fff7a10)

/**
 * @brief   Available number of ADC devices
 */
#if defined(CPU_LINE_STM32F401xE) || defined(CPU_LINE_STM32F410Rx) \
    || defined(CPU_LINE_STM32F411xE) || defined(CPU_LINE_STM32F412Zx) \
    || defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#define ADC_DEVS            (1U)
#elif defined(CPU_LINE_STM32F405xx)  || defined(CPU_LINE_STM32F407xx) \
    || defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F429xx) \
    || defined(CPU_LINE_STM32F437xx) || defined(CPU_LINE_STM32F446xx)
#define ADC_DEVS            (3U)
#endif

#ifndef DOXYGEN
/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x03000000,     /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x02000000,     /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01000000,     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x00000000,     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 1,              /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 2               /**< ADC resolution: 16 bit (not supported)*/
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F4_PERIPH_CPU_H */
/** @} */
