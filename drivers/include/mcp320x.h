/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp320x MCP3204/MCP3208 12-Bit A/D Converter
 * @ingroup     drivers_sensors
 * @brief       Driver for the Microchip MCP320x ADC.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MCP320x ADC driver.
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef MCP320X_H
#define MCP320X_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/spi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Device type that is connected
 */
typedef enum {
    MCP3204                 = 4,
    MCP3208                 = 8
} mcp320x_chip_t;

/**
 * @brief Device descriptor for MCP320x ADC.
 */
typedef struct {
    spi_t spi;              /**< SPI device, the ADC is connected to */
    gpio_t cs;              /**< chip select line to the ADC */
    bool initialized;       /**< ADC status, true if ADC is initialized */
    mcp320x_chip_t type;    /**< defining if we use the MCP3204 or MCP3208 */
} mcp320x_t;

/**
 * @brief The differential modes of the MCP320x
 */
typedef enum {
    CH0_MIN_CH1,
    CH1_MIN_CH0,
    CH2_MIN_CH3,
    CH3_MIN_CH2,
    CH4_MIN_CH5,
    CH5_MIN_CH4,
    CH6_MIN_CH7,
    CH7_MIN_CH6
} mcp320x_diff_mode_t;

/**
 * @brief Initialize the MCP320x ADC driver.
 *
 * @param[out] dev          device descriptor of ADC to initialize
 * @param[in]  spi          SPI bus the ADC is connected to
 * @param[in]  cs           chip select gpio to the ADC
 * @param[in]  type         the MCP320x type
 *
 * @return                  0 on success
 * @return                  -1 on failure
 */
int mcp320x_init(mcp320x_t *dev, spi_t spi, gpio_t cs, mcp320x_chip_t type);

/**
 * @brief Get the measured value at the given channel in counts
 *
 * @param[in] dev           device descriptor of the ADC to read
 * @param[in] channel       the channel to measure
 *
 * @return                  >= 0 on success, counts on the input
 * @return                  -1 on failure
 */
int mcp320x_read_single(mcp320x_t *dev, int channel);

/**
 * @brief Measure differential between two inputs
 *
 * @param[in] dev           device descriptor of the ADC to read
 * @param[in] mode          the channels we want to differentiate
 *
 * @return                  >= 0 on success, counts between the two inputs
 * @return                  -1 on failure
 */
int mcp320x_read_differential(mcp320x_t *dev, mcp320x_diff_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
