/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd21
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define mandatory GPIO types for NRF51822 CPUs
 * @{
 */
#define OVRD_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO(x, y)          (((gpio_t)(&PORT->Group[x])) | y)

/**
 * @brief   Available ports on the SAMD21
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
};

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
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] dev   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
int gpio_init_mux(gpio_t dev, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
