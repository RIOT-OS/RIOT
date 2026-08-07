/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   drivers_ads131m02 ADS131M02 ADC device driver
 * @ingroup    drivers_sensors
 * @brief      SPI Analog-to-Digital Converter device driver
 *
 * @{
 *
 * @file
 * @brief      ads131m02 ADC device driver
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

/**
 * @name    Channel bitmask values for @ref ads131m02_sample
 * @{
 */
#define ADS131M02_CHANNEL_0     (1 << 0)    /**< select ADC channel 0 */
#define ADS131M02_CHANNEL_1     (1 << 1)    /**< select ADC channel 1 */
/** @} */

/**
 * @brief   ADS131M02 params
 */
typedef struct {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t ready_pin;       /**< GPIO pin conected to DRDY */
    gpio_t sync_pin;        /**< GPIO pin conected to SYNC / RESET */
} ads131m02_params_t;

typedef struct {
    const ads131m02_params_t *params;   /**< device driver configuration */
    mutex_t ready;                      /**< wait for data to be ready */
} ads131m02_t;

/**
 * @brief   Enable or disable the external reference clock (CLKIN)
 *
 * If the ADS131M02 is driven from an external reference clock rather
 * than its internal oscillator, boards providing that clock should
 * override this function to enable it before sampling and disable it
 * again once sampling is done. The default implementation does nothing.
 *
 * @param[in] dev       device descriptor
 * @param[in] on        true to enable the clock, false to disable it
 */
void board_ads131m02_clk_ctrl(ads131m02_t *dev, bool on);

/**
 * @brief   Initialize an ADS131M02 ADC device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int ads131m02_init(ads131m02_t *dev, const ads131m02_params_t *params);

/**
 * @brief   Sample data from the ADS131M02 ADC
 *
 * This enables the requested channels, waits for the configured number
 * of conversions to complete (signaled via the DRDY / ready interrupt)
 * and stores the raw 24-bit (sign-extended into 32 bit) conversion
 * results in the given buffers. The ADC is disabled again once
 * sampling is done.
 *
 * @param[in,out] dev       device descriptor
 * @param[in]  channels     bitmask of channels to sample, see
 *                          @ref ADS131M02_CHANNEL_0 / @ref ADS131M02_CHANNEL_1
 * @param[out] chan0        buffer to store channel 0 samples in, may be
 *                          NULL if @ref ADS131M02_CHANNEL_0 is not set
 * @param[out] chan1        buffer to store channel 1 samples in, may be
 *                          NULL if @ref ADS131M02_CHANNEL_1 is not set
 * @param[in]  len          number of samples to acquire
 *
 * @return  number of samples that were read on success
 * @return  <0 on error
 */
int ads131m02_sample(ads131m02_t *dev, uint8_t channels, uint32_t *chan0, uint32_t *chan1, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
