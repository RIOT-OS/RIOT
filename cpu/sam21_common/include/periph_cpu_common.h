/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam21_common
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_COMMON_H
#define CPU_PERIPH_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Use shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

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
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT->Group[x])) | y)

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
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
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] dev   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t dev, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_COMMON_H */
/** @} */
