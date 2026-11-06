/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_max31855 MAX31855 Thermocouple-to-Digital Converter driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the SPI Thermocouple-to-Digital Converter MAX31855.
 *
 *              The MAX31855 performs cold-junction compensation and digitizes
 *              the signal from a K-, J-, N-, T-, S-, R-, or E-type
 *              thermocouple. The data is output in a signed 14-bit,
 *              SPI-compatible, read-only format. This converter resolves
 *              temperatures to 0.25°C, allows readings as high as +1800°C and
 *              as low as -270°C, and exhibits thermocouple accuracy of ±2°C
 *              for temperatures ranging from -200°C to +700°C for K-type
 *              thermocouples.
 *
 * @note See the datasheet for more information:
 * https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31855.pdf
 *
 * @note This driver doesn't require a MOSI line, as the MAX31855 is a read-only.
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"

/* Declare the API of the driver */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;          /**< SPI device */
    spi_cs_t cs_pin;    /**< Chip select pin */
} max31855_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    const max31855_params_t *params;  /**< device configuration */
} max31855_t;

/**
 * @brief   Fault status of the MAX31855
 */
typedef enum {
    MAX31855_FAULT_VCC_SHORT    = 0,    /**< VCC short-circuit */
    MAX31855_FAULT_GND_SHORT    = 1,    /**< GND short-circuit */
    MAX31855_FAULT_OPEN_CIRCUIT = 2,    /**< Open circuit */
    MAX31855_FAULT_NO_FAULT     = 3     /**< No fault */
} max31855_fault_t;

/**
 * @brief   Data structure for the MAX31855
 */
typedef struct {
    int32_t thermocouple_temperature;   /**< Thermocouple temperature in centi degrees C */
    int32_t internal_temperature;       /**< Internal temperature in centi degrees C */
    max31855_fault_t fault;             /**< Fault status */
} max31855_data_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 *
 * @retval    0 on success
 * @retval    -ENXIO invalid SPI device
 * @retval    -EINVAL invalid SPI CS pin/line
 */
int max31855_init(max31855_t *dev, const max31855_params_t *params);

/**
 * @brief   Parse the raw data from the MAX31855 to the data structure
 *
 * @param[in]  raw_data    Raw data from the MAX31855
 * @param[out] data        Pointer to the data structure.
 *
 * @pre                    @p data must not be NULL
 */
void max31855_raw_to_data(uint32_t raw_data, max31855_data_t *data);

/**
 * @brief   Read data from the MAX31855. This is a shortcut to read raw data
 *        and parse it to the data structure.
 *
 * @param[in]  dev         Device descriptor of the driver
 * @param[out] data        Pointer to the data structure.
 *
 * @pre                    @p dev and @p data must not be NULL
 *
 * @retval   0 on success
 * @retval   -EIO if there is an error detected by the MAX31855. For a detailed
 *      error description, check the fault field of the data structure. On this
 *      case, temperature fields are not valid.
 */
int max31855_read(max31855_t *dev, max31855_data_t *data);

/**
 * @brief   Read raw data from the MAX31855
 *
 * @param[in]  dev         Device descriptor of the driver
 * @param[out] data        Pointer where to store the raw data.
 *
 * @pre                    @p dev and @p data must not be NULL
 */
void max31855_read_raw(max31855_t *dev, uint32_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
