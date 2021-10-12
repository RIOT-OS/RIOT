/*
 * Copyright (C) 2015 Engineering-Spirit
 * Copyright (C) 2016 OTA keys S.A.
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
 * @brief           STM32F2 CPU specific definitions for internal peripheral handling
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Aurelien Gonce <aurelien.gonce@altran.fr>
 */

#ifndef PERIPH_F2_PERIPH_CPU_H
#define PERIPH_F2_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

/**
 * @brief   Readout Protection (RDP) option bytes
 */
#define STM32_OPTION_BYTES   ((uint32_t*) 0x1FFFC000)
#define GET_RDP(x) ((x & 0xFF00) >> 8)

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x03000000,  /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x02000000,  /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01000000,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x00000000,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 1,           /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 2            /**< ADC resolution: 16 bit (not supported)*/
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F2_PERIPH_CPU_H */
/** @} */
