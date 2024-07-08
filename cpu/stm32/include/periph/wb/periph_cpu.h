/*
 * Copyright (C) 2019 Inria
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
 * @brief           STM32WB CPU specific definitions for internal peripheral handling
 *
 * @author          Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#ifndef PERIPH_WB_PERIPH_CPU_H
#define PERIPH_WB_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#if defined(ADC3)
#define ADC_DEVS            (3U)
#elif defined(ADC2)
#define ADC_DEVS            (2U)
#elif defined(ADC1)
#define ADC_DEVS            (1U)
#else
#error "Can't determine the number of ADC devices"
#endif

#if defined(CPU_MODEL_STM32WB55RG)
/**
 * @brief   ADC voltage regulator start-up time [us]
 */
#define ADC_T_ADCVREG_STUP_US (20)
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
    ADC_RES_6BIT  = (ADC_CFGR_RES),   /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (ADC_CFGR_RES_1), /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (ADC_CFGR_RES_0), /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x0),            /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0x1),            /**< not applicable */
    ADC_RES_16BIT = (0x2)             /**< not applicable */
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

#endif /* PERIPH_WB_PERIPH_CPU_H */
/** @} */
