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

#ifndef DOXYGEN
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
#endif /* ndef DOXYGEN */

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
 * @brief   Possible pad selections for SERCOM RX (inspired by Arduino)
 */
typedef enum {
    SERCOM_RX_PAD_0 = 0x0,  /**< select pad 0 */
    SERCOM_RX_PAD_1 = 0x1,  /**< select pad 1 */
    SERCOM_RX_PAD_2 = 0x2,  /**< select pad 2 */
    SERCOM_RX_PAD_3 = 0x3,  /**< select pad 3 */
} sercom_rxpad_t;

/**
 * @brief   Possible pad selections for SERCOM UART TX (inspired by Arduino)
 */
typedef enum {
    UART_TX_PAD_0 = 0x0,    /**< select pad 0, only UART */
    UART_TX_PAD_2 = 0x1,    /**< select pad 2, only UART */
    UART_TX_RTS_CTS_PAD_0_2_3 = 0x2,    /**< select pad 0, 2 and 3, only UART, TX on PAD0, RTS on PAD2 and CTS on PAD3 */
} sercom_uart_txpad_t;

/**
 * @brief   Possible pad selections for SERCOM SPI output (inspired by Arduino)
 */
typedef enum {
    SPI_PAD_0_SCK_1 = 0,    /**< select pad 0, SCK pad1, only SPI */
    SPI_PAD_2_SCK_3 = 1,    /**< select pad 2, SCK pad3, only SPI */
    SPI_PAD_3_SCK_1 = 2,    /**< select pad 3, SCK pad1, only SPI */
    SPI_PAD_0_SCK_3 = 3,    /**< select pad 0, SCK pad3, only SPI */
} sercom_spi_txpad_t;

/**
 * @brief   Possible selections for SERCOM SPI data size (inspired by Arduino)
 */
typedef enum
{
    SPI_CHAR_SIZE_8_BITS = 0x0ul,
    SPI_CHAR_SIZE_9_BITS = 0x1ul,
} sercom_spi_charsize_t;

/**
 * @brief   Possible selections for SERCOM data (bit) order (inspired by Arduino)
 */
typedef enum
{
    MSB_FIRST = 0,
    LSB_FIRST = 1,
} sercom_data_order_t;

/**
 * @brief   Possible selections for SERCOM SPI clock mode (inspired by Arduino)
 */
typedef enum
{
    SERCOM_SPI_MODE_0 = 0,      // CPOL : 0  | CPHA : 0
    SERCOM_SPI_MODE_1 = 1,      // CPOL : 0  | CPHA : 1
    SERCOM_SPI_MODE_2 = 2,      // CPOL : 1  | CPHA : 0
    SERCOM_SPI_MODE_3 = 3,      // CPOL : 1  | CPHA : 1
} sercom_spi_clockmode_t;

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Set up PORT pin for SERCOM usage
 *
 * This function initializes the pin for SERCOM usage. It is derived
 * from Arduino's pinPeripheral() in wiring_private.c
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_sercom(gpio_t pin, gpio_mux_t mux);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_COMMON_H */
/** @} */
