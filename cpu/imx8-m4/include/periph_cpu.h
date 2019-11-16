/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_imx8-m4
 * @brief           Common CPU specific definitions for IMX8-CM4
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for IMX8-CM4
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Override GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Macro for accessing GPIO pins
 * @{
 */
#define GPIO_PIN(x, y)          ((gpio_t) ((uint32_t)GPIO1_BASE) | ((x-1) << 16)| y)


/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: PD(0) or PU(1)
 * - bit 1: input enable
 * - bit 2: pull enable
 */
#define GPIO_MODE(pr, ie, pe)   (pr | (ie << 1) | (pe << 2))

/**
 * @brief   Override GPIO modes
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 1, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(1, 1, 1),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(0, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = 0xfe,                  /**< not supported by HW */
    GPIO_OD_PU = 0xff                   /**< not supported by HW */
} gpio_mode_t;

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOWLEVEL = 0,
    GPIO_HIGHLEVEL = 1,
    GPIO_FALLING = 3,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = -1           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */


/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
typedef enum {
    GPIO_MUX_A = 0x0,       /**< select peripheral function A */
    GPIO_MUX_B = 0x1,       /**< select peripheral function B */
    GPIO_MUX_C = 0x2,       /**< select peripheral function C */
    GPIO_MUX_D = 0x3,       /**< select peripheral function D */
    GPIO_MUX_E = 0x4,       /**< select peripheral function E */
    GPIO_MUX_F = 0x5,       /**< select peripheral function F */
    GPIO_MUX_G = 0x6,       /**< select peripheral function G */
    GPIO_MUX_H = 0x7,       /**< select peripheral function H */
} gpio_mux_t;

/**
 * @brief   UART device configuration
 */
typedef struct {
    UART_Type *dev;         /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
    uint32_t clk_src;       /**< GCLK source which supplys SERCOM */
} uart_conf_t;


/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Timer device configuration
 */
typedef struct {
    GPT_Type *dev;          /**< pointer to the used Timer device */
    IRQn_Type irq;          /**< IRQ# of Timer Interrupt */
    uint8_t clksrc;         /**< clock source which supplys IP */
    uint16_t prescaler;     /**< prescaler used by the Timer */
    uint16_t flags;         /**< flags for CTRA, e.g. TC_CTRLA_MODE_COUNT32 */
} gpt_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
