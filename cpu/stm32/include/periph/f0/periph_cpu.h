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
 * @brief           STM32F0 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#if defined(CPU_LINE_STM32F030x4) || defined(CPU_LINE_STM32F030x6) || \
    defined(CPU_LINE_STM32F030x8) || defined(CPU_LINE_STM32F031x6) || \
    defined(CPU_LINE_STM32F051x8)
#define STM32_BOOTLOADER_ADDR   (0x1FFFEC00)
#elif defined(CPU_LINE_STM32F072xB) || defined(CPU_LINE_STM32F070xB)
#define STM32_BOOTLOADER_ADDR   (0x1FFFC800)
#elif defined(CPU_LINE_STM32F030xC) || defined(CPU_LINE_STM32F091xC)
#define STM32_BOOTLOADER_ADDR   (0x1FFFD800)
#elif defined(CPU_LINE_STM32F042x6)
#define STM32_BOOTLOADER_ADDR   (0x1FFFC400)
#endif

/**
 * @brief   Readout Protection (RDP) option bytes
 */
#define STM32_OPTION_BYTES   ((uint32_t*) 0x1FFFF800)
#define GET_RDP(x) (x & 0xFF)

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

#ifdef __cplusplus
}
#endif

/** @} */
