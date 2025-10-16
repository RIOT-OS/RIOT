/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32H7 CPU specific definitions for internal peripheral handling
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
#define STM32_BOOTLOADER_ADDR   (0x1FF00000)

/**
 * @brief   Readout Protection (RDP) option bytes
 */
/* Will be defined in a future commit */

/**
 * @brief   Available number of ADC devices
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
 * @brief   ADC voltage regulator start-up time [us]
 */
#define ADC_T_ADCVREG_STUP_US (10)

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (0x01),                     /**< not applicable */
    ADC_RES_8BIT  = (0x07 << ADC_CFGR_RES_Pos), /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (0x03 << ADC_CFGR_RES_Pos), /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x06 << ADC_CFGR_RES_Pos), /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0x05 << ADC_CFGR_RES_Pos), /**< ADC resolution: 14 bit */
    ADC_RES_16BIT = (0x00 << ADC_CFGR_RES_Pos)  /**< ADC resolution: 16 bit */
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
