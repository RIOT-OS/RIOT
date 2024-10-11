/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 * Copyright (C) 2015 Hamburg University of Applied Sciences
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
 * @brief           STM32L1 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef PERIPH_L1_PERIPH_CPU_H
#define PERIPH_L1_PERIPH_CPU_H

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
#define STM32_OPTION_BYTES   ((uint32_t*) 0x1FF80000)
#define GET_RDP(x) (x & 0xFF)

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (ADC_CR1_RES_0 | ADC_CR1_RES_1),    /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (ADC_CR1_RES_1),                    /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (ADC_CR1_RES_0),                    /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x00),                             /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xfe),                             /**< not applicable */
    ADC_RES_16BIT = (0xff)                              /**< not applicable */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @name    EEPROM configuration
 * @{
 */
#define EEPROM_START_ADDR          (0x08080000)
#if defined(CPU_MODEL_STM32L152RE)
#define EEPROM_SIZE                (16384UL)  /* 16kB */
#elif defined(CPU_MODEL_STM32L151RC)
#define EEPROM_SIZE                (8192U)    /* 8kB */
#elif defined(CPU_MODEL_STM32L151CB) || defined(CPU_MODEL_STM32L151CB_A)
#define EEPROM_SIZE                (4096U)    /* 4kB */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_L1_PERIPH_CPU_H */
