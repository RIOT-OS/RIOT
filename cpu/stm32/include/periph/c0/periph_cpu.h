/*
 * SPDX-FileCopyrightText: 2024 BISSELL Homecare, Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32C0 CPU specific definitions for internal peripheral handling
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
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

#ifdef __cplusplus
}
#endif

/** @} */
