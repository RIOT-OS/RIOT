/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32F0 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_F0_PERIPH_CPU_H
#define PERIPH_F0_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1ffff7ac)

#ifndef DOXYGEN
/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (0x3 << 3),     /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (0x2 << 3),     /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (0x1 << 3),     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x0 << 3),     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xfe),         /**< not applicable */
    ADC_RES_16BIT = (0xff)          /**< not applicable */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F0_PERIPH_CPU_H */
/** @} */
