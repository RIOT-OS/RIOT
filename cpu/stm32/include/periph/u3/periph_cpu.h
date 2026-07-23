/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       STM32U3 family specific peripheral CPU definitions
 *
 * @author      Adarsh Nair Mullachery <adarsh.mullachery@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x0BF8F000)

/**
 * @brief   Number of ADC controller instances (CMSIS @c ADCx_BASE symbols)
 */
#if defined(ADC4)
#  define ADC_DEVS          (4U)
#elif defined(ADC3)
#  define ADC_DEVS          (3U)
#elif defined(ADC2)
#  define ADC_DEVS          (2U)
#elif defined(ADC1)
#  define ADC_DEVS          (1U)
#endif

/**
 * @brief   ADC voltage regulator start-up time [µs] (RM0487, @c t_ADCVREG_STUP)
 */
#define ADC_T_ADCVREG_STUP_US   (20U)

/**
 * @brief   Mask of @c ADC_CFGR1.RES bits — used by @ref adc_f3_h7_u3.c @c adc_sample()
 */
#define ADC_CFGR_RES            ADC_CFGR1_RES_Msk

/**
 * @brief   Highest ADC channel index described in SMPR/SQR bit maps (RM0487)
 */
#define ADC_CONF_CHAN_MAX       (19U)

/**
 * @name   ADC resolution (@c ADC_CFGR1.RES)
 * @{
 * @note 14-bit and 16-bit values are RIOT placeholders; @c adc_sample() rejects
 *       them unless a future driver adds oversampling / precision modes.
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (ADC_CFGR1_RES),           /**< 6 bit */
    ADC_RES_8BIT  = (ADC_CFGR1_RES_1),         /**< 8 bit */
    ADC_RES_10BIT = (ADC_CFGR1_RES_0),         /**< 10 bit */
    ADC_RES_12BIT = (0x00),                    /**< 12 bit */
    ADC_RES_14BIT = (0xfe),                    /**< not supported via CFGR1 alone */
    ADC_RES_16BIT = (0xff)                     /**< not supported via CFGR1 alone */
} adc_res_t;
/** @} */

/**
 * @name   Constants for internal VBAT ADC line
 * @{
 */
#define VBAT_ADC_RES        ADC_RES_12BIT
#define VBAT_ADC_MAX        4095
/** @} */

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif
