/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32l1
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Starting address of the CPU ID
 */
#ifdef CPU_MODEL_STM32L151RBA
#define CPUID_ADDR          (0x1ff80050)
#else
#define CPUID_ADDR          (0x1ff800d0)
#endif

/**
 * @brief   Available ports on the STM32L1 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 6,             /**< port F */
    PORT_G = 7,             /**< port G */
    PORT_H = 5,             /**< port H */
};

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

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

/**
 * @name    EEPROM configuration
 * @{
 */
#define EEPROM_START_ADDR          (0x08080000)
#if defined(CPU_MODEL_STM32L152RE)
#define EEPROM_SIZE                (16384UL)  /* 16kB */
#elif defined(CPU_MODEL_STM32L151RC)
#define EEPROM_SIZE                (8192U)    /* 8kB */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
