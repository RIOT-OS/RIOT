/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam0_common
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
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
 * @brief   Available values for SERCOM UART RX pad selection
 */
typedef enum {
    UART_PAD_RX_0 = 0x0,    /**< use pad 0 for RX line */
    UART_PAD_RX_1 = 0x1,    /**< select pad 1 */
    UART_PAD_RX_2 = 0x2,    /**< select pad 2 */
    UART_PAD_RX_3 = 0x3,    /**< select pad 3 */
} uart_rxpad_t;

/**
 * @brief   Available values for SERCOM UART TX pad selection
 */
typedef enum {
    UART_PAD_TX_0             = 0x0,    /**< select pad 0 */
    UART_PAD_TX_2             = 0x1,    /**< select pad 2 */
    UART_PAD_TX_0_RTS_2_CTS_3 = 0x2,    /**< TX is pad 0, on top RTS on pad 2
                                         *   and CTS on pad 3 */
} uart_txpad_t;

/**
 * @brief   Available values for SERCOM SPI MISO pad selection
 */
typedef enum {
    SPI_PAD_MISO_0 = 0x0,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_1 = 0x1,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_2 = 0x2,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_3 = 0x3,       /**< use pad 0 for MISO line */
} spi_misopad_t;

/**
 * @brief   Available values for SERCOM SPI MOSI and SCK pad selection
 */
typedef enum {
    SPI_PAD_MOSI_0_SCK_1 = 0x0, /**< use pad 0 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_2_SCK_3 = 0x1, /**< use pad 2 for MOSI, pad 3 for SCK */
    SPI_PAD_MOSI_3_SCK_1 = 0x2, /**< use pad 3 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_0_SCK_3 = 0x3, /**< use pad 0 for MOSI, pad 3 for SCK */
} spi_mosipad_t;

/**
 * @brief   Override SPI modes
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0x0,       /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0x1,       /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = 0x2,       /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = 0x3        /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ =   100000U, /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ =   400000U, /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   =  1000000U, /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   =  5000000U, /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000U  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief   SPI device configuration
 */
typedef struct {
    SercomSpi *dev;         /**< pointer to the used SPI device */
    gpio_t miso_pin;        /**< used MISO pin */
    gpio_t mosi_pin;        /**< used MOSI pin */
    gpio_t clk_pin;         /**< used CLK pin */
    gpio_mux_t miso_mux;    /**< alternate function for MISO pin (mux) */
    gpio_mux_t mosi_mux;    /**< alternate function for MOSI pin (mux) */
    gpio_mux_t clk_mux;     /**< alternate function for CLK pin (mux) */
    spi_misopad_t miso_pad; /**< pad to use for MISO line */
    spi_mosipad_t mosi_pad; /**< pad to use for MOSI and CLK line */
} spi_conf_t;

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline int sercom_id(void *sercom)
{
    return ((((uint32_t)sercom) >> 10) & 0x7) - 2;
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_COMMON_H */
/** @} */
