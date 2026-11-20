/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Hamburg University of Applied Sciences
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal peripheral handling
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 */

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @name    RTT configuration
 * @{
 */
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (CHIP_FREQ_XTAL_32K)          /* in Hz */
#define RTT_MIN_FREQUENCY   (1) /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/** @} */

/**
 * @name    ADC configuration, valid for all boards using this CPU
 *
 * The sam3 has a fixed mapping of ADC pins and a fixed number of ADC channels,
 * so this ADC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 */
#define ADC_NUMOF           (16U)

/**
 * @brief   DAC configuration, valid for all boards using this CPU
 *
 * The sam3 has a fixed mapping of DAC pins and a fixed number of DAC channels,
 * so this DAC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 *
 * The sam3's DAC channels are mapped to the following fixed pins:
 * - line 0 (ch0): PB15
 * - line 1 (ch1): PB16
 */
#define DAC_NUMOF           (2U)

#ifndef DOXYGEN
/**
 * @brief   Override default SPI modes
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = (SPI_CSR_NCPHA),                   /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = (0),                               /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = (SPI_CSR_CPOL | SPI_CSR_NCPHA),    /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = (SPI_CSR_CPOL)                     /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override default SPI clock values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = (100000),                      /**< 100KHz */
    SPI_CLK_400KHZ = (400000),                      /**< 400KHz */
    SPI_CLK_1MHZ   = (1000000),                     /**< 1MHz */
    SPI_CLK_5MHZ   = (5000000),                     /**< 5MHz */
    SPI_CLK_10MHZ  = (10000000)                     /**< 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifndef DOXYGEN
/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0x1,                    /**< not applicable */
    ADC_RES_8BIT  = 0x2,                    /**< not applicable */
    ADC_RES_10BIT = ADC_MR_LOWRES_BITS_10,  /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_MR_LOWRES_BITS_12,  /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0x4,                    /**< not applicable */
    ADC_RES_16BIT = 0x8                     /**< not applicable */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   PWM channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin connected to the channel */
    uint8_t hwchan;         /**< the HW channel used for a logical channel */
} pwm_chan_conf_t;

/**
 * @brief   SPI configuration data
 */
typedef struct {
    Spi *dev;               /**< SPI module to use */
    uint8_t id;             /**< corresponding ID of that module */
    gpio_t clk;             /**< pin mapped to the CLK line */
    gpio_t mosi;            /**< pin mapped to the MOSI line */
    gpio_t miso;            /**< pin mapped to the MISO line */
    gpio_mux_t mux;         /**< pin MUX setting */
} spi_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
