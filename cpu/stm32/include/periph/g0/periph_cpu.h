/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32G0 CPU specific definitions for internal peripheral handling
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (ADC_CFGR1_RES),    /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (ADC_CFGR1_RES_1),  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (ADC_CFGR1_RES_0),  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x00),             /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xfe),     /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = (0xff)      /**< ADC resolution: 16 bit (not supported) */
} adc_res_t;
/** @} */

/**
 * @name   Constants for internal VBAT ADC line
 * @{
 */
#define VBAT_ADC_RES        ADC_RES_12BIT
#define VBAT_ADC_MAX        4095
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   TIM6, DAC and LPTIM1 share the same interrupt
 */
#if defined(CPU_LINE_STM32G0B1xx) || defined(CPU_LINE_STM32G081xx) || \
    defined(CPU_LINE_STM32G071xx) || defined(CPU_LINE_STM32G0C1xx)
#define TIM6_DAC_LPTIM1_SHARED_IRQ
#endif

#ifdef __cplusplus
}
#endif

/** @} */
