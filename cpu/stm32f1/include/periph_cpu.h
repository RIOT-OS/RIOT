/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f1
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1ffff7e8)

/**
 * @brief   All timers for the STM32F1 have 4 CC channels
 */
#define TIMER_CHANNELS      (4U)

/**
 * @brief   All timers have a width of 16-bit
 */
#define TIMER_MAXVAL        (0xffff)

/**
 * @brief   Define the number of available PM modes
 */
#define PM_NUM_MODES        (2U)

/**
 * @brief  Define the config flag for stop mode
 */
#define PM_STOP_CONFIG      (PWR_CR_LPDS)

/**
 * @brief   Available ports on the STM32F1 family
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    GPIO_EXT_PORT = 7       /**< first GPIO extender port */
};

/**
 * @brief   Available ports on the STM32F1 family as GPIO register definitions
 */
#define GPIO_CPU_PORTS \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_A)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_B)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_C)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_D)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_E)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_F)), \
    GPIO_CPU_PORT(GPIO_CPU_PORT_ADDR(PORT_G)),

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
