/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
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
 * @brief           STM32F4 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#if defined(CPU_LINE_STM32F401xE) || defined(CPU_LINE_STM32F410Rx) \
    || defined(CPU_LINE_STM32F411xE) || defined(CPU_LINE_STM32F412Zx) \
    || defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#define ADC_DEVS            (1U)
#elif defined(CPU_LINE_STM32F405xx)  || defined(CPU_LINE_STM32F407xx) \
    || defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F429xx) \
    || defined(CPU_LINE_STM32F439xx) || defined(CPU_LINE_STM32F437xx) \
    || defined(CPU_LINE_STM32F446xx) || defined(CPU_LINE_STM32F469xx)
#define ADC_DEVS            (3U)
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#if defined(CPU_LINE_STM32F423xx)
#define STM32_BOOTLOADER_ADDR   (0x1FF00000)
#else
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)
#endif

/**
 * @brief   Readout Protection (RDP) option bytes
 */
#define STM32_OPTION_BYTES   ((uint32_t*) 0x1FFFC000)
#define GET_RDP(x) ((x & 0xFF00) >> 8)

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (ADC_CR1_RES),      /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (ADC_CR1_RES_1),    /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (ADC_CR1_RES_0),    /**< ADC resolution: 10 bit */
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

#ifdef __cplusplus
}
#endif

/** @} */
