/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_nrf9160
 * @{
 *
 * @file
 * @brief           nRF9160 specific definitions for handling peripherals
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF9160 CPUs
 */
#define CLOCK_CORECLOCK     MHZ(64)

/**
 * @name    Peripheral aliases for the secure and the non-secure world
 *
 * The nRF9160 exposes every peripheral twice, once for each security state.
 * Board configuration refers to the neutral name, which resolves to the alias
 * matching the image that is being built.
 * @{
 */
#ifdef NRF_TRUSTZONE_NONSECURE
#  define NRF_P0            NRF_P0_NS       /**< GPIO port 0 */
#  define NRF_PWM0          NRF_PWM0_NS     /**< PWM 0 */
#  define NRF_SPIM2         NRF_SPIM2_NS    /**< SPI master 2 */
#  define NRF_TIMER0        NRF_TIMER0_NS   /**< Timer 0 */
#  define NRF_TIMER1        NRF_TIMER1_NS   /**< Timer 1 */
#  define NRF_TWIM3         NRF_TWIM3_NS    /**< I2C master 3 */
#  define NRF_UARTE0        NRF_UARTE0_NS   /**< UART 0 */
#  define NRF_UARTE1        NRF_UARTE1_NS   /**< UART 1 */
#else
#  define NRF_P0            NRF_P0_S        /**< GPIO port 0 */
#  define NRF_PWM0          NRF_PWM0_S      /**< PWM 0 */
#  define NRF_SPIM2         NRF_SPIM2_S     /**< SPI master 2 */
#  define NRF_TIMER0        NRF_TIMER0_S    /**< Timer 0 */
#  define NRF_TIMER1        NRF_TIMER1_S    /**< Timer 1 */
#  define NRF_TWIM3         NRF_TWIM3_S     /**< I2C master 3 */
#  define NRF_UARTE0        NRF_UARTE0_S    /**< UART 0 */
#  define NRF_UARTE1        NRF_UARTE1_S    /**< UART 1 */
#endif
/** @} */

/**
 * @brief    Peripheral clock speed (fixed to 16MHz for nRF9160 based CPUs)
 */
#define PERIPH_CLOCK        MHZ(16)

#ifdef __cplusplus
}
#endif

/** @} */
