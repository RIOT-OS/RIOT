/*
 * SPDX-FileCopyrightText: 2025 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_sam_common
 * @brief           Common CPU specific definitions for all SAM3/SAM4x based CPUs
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for all SAM3/SAM4x based CPUs
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO_UNDEF          (0xffffffff)

#define GPIO_PIN(x, y)      (((uint32_t)PIOA + (x << 9)) | y)
#endif /* DOXYGEN */

/**
 * @brief   We use one channel for each defined timer
 *
 * While the peripheral provides three channels, the current interrupt
 * flag handling leads to a race condition where calling timer_clear() on one
 * channel can disable a pending flag for other channels.
 * Until resolved, limit the peripheral to only one channel.
 */
#define TIMER_CHANNEL_NUMOF (1)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: in/out
 * - bit 1: PU enable
 * - bit 2: OD enable
 */
#define GPIO_MODE(io, pu, od)   (io | (pu << 1) | (od << 2))

#ifndef DOXYGEN
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = 0xf,                   /**< not supported by HW */
    GPIO_IN_PU = GPIO_MODE(0, 1, 0),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = GPIO_MODE(1, 0, 1),    /**< OD */
    GPIO_OD_PU = GPIO_MODE(1, 1, 1),    /**< OD with pull-up */
} gpio_mode_t;

#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
#endif /* ndef DOXYGEN */

/**
 * @brief   GPIO mux configuration
 */
typedef enum {
    GPIO_MUX_A = 0,         /**< alternate function A */
    GPIO_MUX_B = 1,         /**< alternate function B */
#ifdef CPU_FAM_SAM4S
    GPIO_MUX_C = 2,         /**< alternate function C */
    GPIO_MUX_D = 3,         /**< alternate function D */
#endif
} gpio_mux_t;

/**
 * @brief Available ports on the SAM3/SAM4S based MCUs
 */
enum {
    PA = 0,                      /**< port A */
    PB = 1,                      /**< port B */
    PC = 2,                      /**< port C */
#ifdef CPU_FAM_SAM3
    PD = 3,                      /**< port D */
#endif
};

/**
 * @brief   Configure the given GPIO pin to be used with the given MUX setting
 *
 * @param[in] pin           GPIO pin to configure
 * @param[in] mux           MUX setting to use
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Timer configuration data
 */
typedef struct {
    Tc *dev;                /**< timer device */
    uint8_t id_ch0;         /**< ID of the timer's first channel */
} timer_conf_t;

/**
 * @brief   UART configuration data
 */
typedef struct {
    Uart *dev;              /**< U(S)ART device used */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    gpio_mux_t mux;         /**< MUX used for pins */
    uint8_t pmc_id;         /**< bit in the PMC register of the device*/
    uint8_t irqn;           /**< interrupt number of the device */
} uart_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
