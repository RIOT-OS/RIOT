/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_msp430fxyz TI MSP430F
 * @ingroup     cpu
 * @brief       Texas Instruments MSP430F family specific code
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdbool.h>
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
 */
#define GPIO_PIN(x, y)      ((gpio_t)(((x & 0xff) << 8) | (1 << (y & 0x07))))

/**
 * @brief   No support for HW chip select...
 */
#define SPI_HWCS(x)         (SPI_CS_UNDEF)

#ifndef DOXYGEN
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
 * @brief   Override SPI mode selection values
 * @{
 */
#define HAVE_SPI_MODE_T
#ifndef SPI_USE_USCI
typedef enum {
    SPI_MODE_0 = (USART_TCTL_CKPH),                         /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0,                                         /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (USART_TCTL_CKPL | USART_TCTL_CKPH),       /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (USART_TCTL_CKPL)                          /**< CPOL=1, CPHA=1 */
} spi_mode_t;
#else
typedef enum {
    SPI_MODE_0 = (USCI_SPI_CTL0_CKPH),                      /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0,                                         /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (USCI_SPI_CTL0_CKPL | USCI_SPI_CTL0_CKPH), /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (USCI_SPI_CTL0_CKPL)                       /**< CPOL=1, CPHA=1 */
} spi_mode_t;
#endif
/** @} */

/**
 * @brief   Override SPI clock speed selection values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100000,    /**< 100KHz */
    SPI_CLK_400KHZ = 400000,    /**< 400KHz */
    SPI_CLK_1MHZ   = 1000000,   /**< 1MHz */
    SPI_CLK_5MHZ   = 5000000,   /**< 5MHz */
    SPI_CLK_10MHZ  = 0,         /**< not supported */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

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
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
