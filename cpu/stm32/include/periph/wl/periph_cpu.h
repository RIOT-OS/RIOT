/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @brief           STM32WL CPU specific definitions for internal peripheral handling
 *
 * @author          Akshai M <akshai.m@fu-berlin.de>
 *
 */

#ifndef PERIPH_WL_PERIPH_CPU_H
#define PERIPH_WL_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   ADC voltage regulator start-up time [us]
 */
#define ADC_T_ADCVREG_STUP_US (20)

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (1U)

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (ADC_CFGR1_RES),   /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (ADC_CFGR1_RES_1), /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (ADC_CFGR1_RES_0), /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x0),            /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0x1),            /**< not applicable */
    ADC_RES_16BIT = (0x2)             /**< not applicable */
} adc_res_t;
/** @} */

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606 ( Table 143 : System memory)
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

/**
 * @name   Constants for internal VBAT ADC line
 * @{
 */
#define VBAT_ADC_RES        ADC_RES_12BIT
#define VBAT_ADC_MAX        4095
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @defgroup        cpu_stm32_wl_debug STM32WL hardware debugging
 * @ingroup         cpu_stm32
 * @{
 */
/**
 * @defgroup        cpu_stm32_wl_debug_subghz_spi STM32WL Sub-GHz SPI debug pins
 * @ingroup         cpu_stm32_wl_debug
 * @{
 */
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT     GPIO_PIN(PORT_A, 7)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT_AF  GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT     GPIO_PIN(PORT_A, 6)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT_AF  GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT      GPIO_PIN(PORT_A, 5)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT_AF   GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT      GPIO_PIN(PORT_A, 4)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT_AF   GPIO_AF13
/** @} */
/** @} */

/**
 * @defgroup        cpu_stm32_wl_debug_subghz_radio STM32WL Sub-GHz Radio debug pins
 * @ingroup         cpu_stm32_wl_debug
 * @{
 */
/*!
 * RF BUSY debug pin definition
 */
#define CPU_STM32WL_SUBGHZ_RF_BUSY              GPIO_PIN(PORT_A, 12)
/*!
 * RF BUSY debug pin alternate function
 */
#define CPU_STM32WL_SUBGHZ_RF_BUSY_AF           GPIO_AF6

/*!
 * RF NRESET debug pin definition
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET      GPIO_PIN(PORT_A, 11)
/*!
 * RF NRESET debug pin alternate function
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET_AF   GPIO_AF13

/*!
 * RF SMPSRDY debug pin definition
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY     GPIO_PIN(PORT_B, 2)
/*!
 * RF SMPSRDY debug pin alternate function
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY_AF  GPIO_AF13

/*!
 * RF LDORDY debug pin definition
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY      GPIO_PIN(PORT_B, 4)
/*!
 * RF LDORDY debug pin alternate function
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY_AF   GPIO_AF13

/*!
 * RF HSE32RDY debug pin definition
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY    GPIO_PIN(PORT_A, 10)
/*!
 * RF HSE32RDY debug pin alternate function
 */
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY_AF GPIO_AF13
/** @} */

/**
 * @defgroup        cpu_stm32_wl_config STM32WL compile time configuration
 * @ingroup         cpu_stm32_wl_debug
 * @ingroup         config_cpu
 * @{
 */
/**
 * @brief Set this to 1 to enable hardware debugging.
 */
#ifdef DOXYGEN
#define CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_WL_PERIPH_CPU_H */
/** @} */
