/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief           STM32F7 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_F7_PERIPH_CPU_H
#define PERIPH_F7_PERIPH_CPU_H

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
#define STM32_OPTION_BYTES    ((uint32_t*) 0x1FFF0000)
#define GET_RDP(x) ((x & 0xFF00) >> 8)

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (3U)

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

#endif /* PERIPH_F7_PERIPH_CPU_H */
/** @} */
