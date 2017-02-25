/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           nRF5x common definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Override macro for defining GPIO pins
 *
 * The port definition is used (and zeroed) to suppress compiler warnings
 */
#define GPIO_PIN(x,y)       ((x & 0) | y)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 4 bit to encode the pin mode:
 * - bit   0: output enable
 * - bit   1: input connect
 * - bit 2+3: pull resistor configuration
 */
#define GPIO_MODE(oe, ic, pr)   (oe | (ic << 1) | (pr << 2))

/**
 * @brief   No support for HW chip select...
 */
#define SPI_HWCS(x)         (SPI_CS_UNDEF)

/**
 * @brief   Declare needed shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
 *
 * We use 4 bit to encode the pin mode:
 * - bit   0: output enable
 * - bit   1: input connect
 * - bit 2+3: pull resistor configuration
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 0, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 0, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(0, 0, 3),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(1, 1, 0),    /**< OUT (push-pull) */
    GPIO_OD    = (0xff),                /**< not supported by HW */
    GPIO_OD_PU = (0xfe)                 /**< not supported by HW */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override GPIO active flank values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING  = 1,       /**< emit interrupt on rising flank */
    GPIO_BOTH    = 3        /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = 0x00,   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x02,   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0xf1,   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xf2,   /**< ADC resolution: 14 bit */
    ADC_RES_16BIT = 0xf3    /**< ADC resolution: 16 bit */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Timer configuration options
 */
typedef struct {
    NRF_TIMER_Type *dev;    /**< timer device */
    uint8_t channels;       /**< number of channels available */
    uint8_t bitmode;        /**< counter width */
    uint8_t irqn;           /**< IRQ number of the timer device */
} timer_conf_t;

/**
 * @brief   Override SPI mode values
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                             /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = SPI_CONFIG_CPHA_Msk,                           /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = SPI_CONFIG_CPOL_Msk,                           /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CONFIG_CPOL_Msk | SPI_CONFIG_CPHA_Msk)    /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = SPI_FREQUENCY_FREQUENCY_K125,  /**< 100KHz */
    SPI_CLK_400KHZ = SPI_FREQUENCY_FREQUENCY_K500,  /**< 400KHz */
    SPI_CLK_1MHZ   = SPI_FREQUENCY_FREQUENCY_M1,    /**< 1MHz */
    SPI_CLK_5MHZ   = SPI_FREQUENCY_FREQUENCY_M4,    /**< 5MHz */
    SPI_CLK_10MHZ  = SPI_FREQUENCY_FREQUENCY_M8     /**< 10MHz */
} spi_clk_t;
/** @} */

/**
 * @brief  SPI configuration values
 */
typedef struct {
    NRF_SPI_Type *dev;  /**< SPI device used */
    uint8_t sclk;       /**< CLK pin */
    uint8_t mosi;       /**< MOSI pin */
    uint8_t miso;       /**< MISO pin */
} spi_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
