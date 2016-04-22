/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H
#define CPU_PERIPH_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Iron out some differences in register and IRQ channel naming between
 *          the different nRFx family members
 * @{
 */
#if defined(CPU_FAM_NRF51)
#define GPIO_BASE           (NRF_GPIO)
#define UART_IRQN           (UART0_IRQn)
#elif defined(CPU_FAM_NRF52)
#define GPIO_BASE           (NRF_P0)
#define UART_IRQN           (UARTE0_UART0_IRQn)
#else
#error "nrf5x_common: no valid value for CPU_FAM_XX defined"
#endif
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Override macro for defining GPIO pins
 *
 * The port definition is used (and zeroed) to suppress compiler warnings
 */
#define GPIO_PIN(x,y)       ((x & 0) | y)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to encode the pin mode:
 * - bit   0: output enable
 * - bit   1: input connect
 * - bit 2+3: pull resistor configuration
 */
#define GPIO_MODE(oe, ic, pr)   (oe | (ic << 1) | (pr << 2))

/**
 * @brief   Override GPIO modes
 *
 * We use 4 bit to encode the pin mode:
 * - bit   0: output enable
 * - bit   1: input connect
 * - bit 2+3: pull resistor configuration
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 0, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 0, 3),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 1, 0),    /**< OUT (push-pull) */
    GPIO_OD    = (0xff),                /**< not supported by HW */
    GPIO_OD_PU = (0xfe)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override GPIO active flank values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING  = 1,       /**< emit interrupt on rising flank */
    GPIO_BOTH    = 3        /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x00,   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x02,   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0xf1,   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xf2,   /**< ADC resolution: 14 bit */
    ADC_RES_16BIT = 0xf3    /**< ADC resolution: 16 bit */
} adc_res_t;
/** @} */

/**
 * @brief   Timer configuration options
 */
typedef struct {
    NRF_TIMER_Type *dev;
    uint8_t channels;
    uint8_t bitmode;
    uint8_t irqn;
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H */
/** @} */
