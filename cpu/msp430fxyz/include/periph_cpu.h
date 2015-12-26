/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#include "cpu.h"
#include "msp430_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffff)

/**
 * @brief   Mandatory function for defining a GPIO pins
 * @{
 */
#define GPIO_PIN(x, y)      ((gpio_t)(((x & 0xff) << 8) | (1 << (y & 0xff))))

/**
 * @brief   Override direction values
 * @{
 */
#define HAVE_GPIO_DIR_T
typedef enum {
    GPIO_DIR_IN =  0x00,    /**< configure pin as input */
    GPIO_DIR_OUT = 0xff,    /**< configure pin as output */
} gpio_dir_t;
/** @} */

/**
 * @brief   Override flank selection values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 0xff,        /**< emit interrupt on falling flank */
    GPIO_RISING  = 0x00,        /**< emit interrupt on rising flank */
    GPIO_BOTH    = 0xab         /**< not supported -> random value*/
} gpio_flank_t;
/** @} */

/**
 * @brief   Available ports on MSP430 platforms
 */
enum {
    P1 = 1,                 /**< PORT 1 */
    P2 = 2,                 /**< PORT 2 */
    P3 = 3,                 /**< PORT 3 */
    P4 = 4,                 /**< PORT 4 */
    P5 = 5,                 /**< PORT 5 */
    P6 = 6,                 /**< PORT 6 */
};

/**
 * @brief   Enable or disable a pin to be used by peripheral modules
 *
 * @param[in] pin       pin to (de-)select
 * @param[in] enable    true for enabling peripheral use, false for disabling it
 */
void gpio_periph_mode(gpio_t pin, bool enable);

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

#endif /* CPU_PERIPH_H_ */
/** @} */
