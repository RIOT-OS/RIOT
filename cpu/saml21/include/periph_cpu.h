/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml21
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define mandatory GPIO types for NRF51822 CPUs
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
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO(x, y)          (((gpio_t)(&PORT->Group[x])) | y)

/**
 * @brief   Available ports on the SAML21 for convenient access
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
};

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
 * @brief   UART device configuration
 */
typedef struct {
    SercomUsart *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
} uart_conf_t;

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline int _sercom_id(SercomUsart *sercom)
{
    return ((((uint32_t)sercom) >> 10) & 0x7) - 2;
}

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] dev   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t dev, gpio_mux_t mux);

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
