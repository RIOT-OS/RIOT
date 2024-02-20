/*
 * Copyright (C) 2024 CNRS, France
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   drivers_abp2 ABP2 series Honeywell pressure and temperature sensor driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      SPI and I2C pressure and temperature sensor driver.
 *
 *
 * ## Description
 *
 * The unit and the range depend on the part number of the sensor.
 * The default parameters assume a 0..160 mbar model.
 * It is the responsibility of the user to use homogeneous units, depending
 * on the actual sensor being used.
 * The pressure range is set when calling #abp2_init().
 *
 * This driver provides @ref drivers_saul capabilities.
 * The functions return 0 on success or a negative value made from \c errno.h on error.
 *
 * ## Usage
 *
 * See `RIOT/tests/drivers/abp2` for an example application using this driver.
 *
 * Add the following modules to the application makefile:
 * \code
 * USEMODULE += abp2
 * USEMODULE += abp2_spi
 * \endcode
 * When the I2C version is supported, `abp2_i2c` can be used for I2C sensors
 * instead of `abp2_spi`.
 *
 * To use the SAUL interface, add also to the makefile:
 * \code
 * USEMODULE += saul_default
 * \endcode
 *
 * Initialize the driver by calling #abp2_init().
 *
 * When a measurement is started on the sensor, its internal ADC converts
 * both pressure and temperature.
 * The conversion takes about 5ms.
 * These two values are retrieved in a single bus transfer.
 *
 * There are three ways to use the driver:
 * - blocking mode : call #abp2_read()
 * - non-blocking mode : call #abp2_read_nb()
 * - the SAUL interface
 *
 * The first one will block for about 5ms and return the latest data, while
 * the second one will return almost immediately but will return data from
 * the previous ADC conversion.
 *
 * ## Caveats
 *
 * This driver currently doesn't support the I2C flavor of these sensors.
 *
 * @{
 *
 * @file
 * @brief      Honeywell ABP2 series pressure and temperature
 *             sensor driver
 *
 * @author     David Picard <david.picard@clermont.in2p3.fr>
 */

#ifndef ABP2_H
#define ABP2_H

#include "saul.h"

#if defined(MODULE_ABP2_SPI)
#include "periph/spi.h"
#include "periph/gpio.h"
#elif defined(MODULE_ABP2_I2C)
#include "periph/i2c.h"
#else
#error "ABP2 driver >> Please add a valid module: either abp2_spi or abp2_i2c"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_ABP2_I2C)
/**
 * @brief   Default I2C slave address for ABP2 devices
 *
 * The sensor address cannot be changed.
 * A different address can only be obtained upon special request to Honeywell.
 */
#define ABP2_ADDR_DEFAULT       (0x28)
#endif

/**
 * @defgroup abp2_statusbits Status bits
 * @{
 *  Macro definitions to manipulate the bits of the status byte.
 *
 * Status byte explanation :
 *
 *  Bit | Meaning                       | Comment                         |
 *  --- | ---                           | ---                             |
 *  7   | always 0                      |                                 |
 *  6   | 1 : device is powered         |                                 |
 *  5   | 1 : device is busy            | Wait longer between 2 commands. |
 *  4   | always 0                      |                                 |
 *  3   | always 0                      |                                 |
 *  2   | 1 : integrity test failed     | Checksum error at power-up.     |
 *  1   | always 0                      |                                 |
 *  0   | 1 : internal math saturation  |                                 |
 */
#define ABP2_STATUS_BUSY        0x20    /**< Sensor is busy. */
#define ABP2_STATUS_MEMERR      0x04    /**< Memory integrity error. */
#define ABP2_STATUS_MATHSAT     0x01    /**< Math saturation error. */
/** A bitwise AND of the status byte with this mask will return true in case of error. */
#define ABP2_STATUS_MASK        (ABP2_STATUS_BUSY | ABP2_STATUS_MEMERR | ABP2_STATUS_MATHSAT)
/** @} */ /* end of abp2_statusbits */

/**
 * @brief   Parameters for ABP2 sensors.
 *
 * The range must be set in thousandths of sensor unit.
 * For example, if the sensor unit is mbar, set the \a rangeMin and \a rangeMax
 * properties in µbar.
 */
typedef struct abp2_params {
#if defined(MODULE_ABP2_SPI)
    spi_t spi;              /**< SPI bus the sensor is connected to. */
    spi_cs_t cs;            /**< CS pin GPIO handle. */
    spi_clk_t clk;          /**< SPI bus clock speed. */
#else
    i2c_t i2c;              /**< I2C bus the device is connected to. */
#endif
    int32_t rangeMin;       /**< Minimum pressure range (thousandths of sensor unit). */
    int32_t rangeMax;       /**< Maximum pressure range (thousandths of sensor unit). */
} abp2_params_t;

/**
 * @brief   Device descriptor for ABP2 sensors.
 */
typedef struct abp2 {
    const abp2_params_t *params;   /**< Sensor parameters. */
} abp2_t;

/**
 * @brief Raw values read from a ABP2 sensor.
 */
typedef struct abp2_raw {
    uint32_t cntPress;      /**< 24-bit digital pressure reading (counts). */
    uint32_t cntTemp;       /**< 24-bit digital temperature reading (counts). */
} abp2_raw_t;

/**
 * @brief   Initialize the ABP2 sensor.
 *
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[in]  params       Sensor parameters. \sa #abp2_params.
 *
 * Assign the parameters \a params to the device descriptor \a dev.
 * Poll the device until the busy flag clears or the operation times out.
 * On SPI version, init the chip select pin.
 *
 * @note The bus is expected to be initialized when calling this function.
 * @note \a params sets the sensor range.
 *
 * @return                  0 on success
 * @return                  \a -EIO on bus error or \a -ETIMEDOUT on timeout.
 */
int abp2_init(abp2_t *dev, const abp2_params_t *params);

/**
 * @brief Let the sensor make one measurement.
 *
 * @param[in]  dev          Sensor device descriptor.
 *
 * This function lets the sensor transition from standby to operation mode
 * and make one measurement, after which it will automatically return to
 * standby mode.
 * Call #abp2_read_raw() to retrieve the data.
 *
 * @return                  0 on success
 * @return                  \a -ENODATA on sensor error.
 */
int abp2_measure(const abp2_t *dev);

/**
 * @brief Measure pressure and temperature (blocking version).
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[out]  press       Pressure value read from the sensor (thousandths of sensor unit).
 * @param[out]  temp        Temperature read from the sensor (milli-degrees Celsius).
 *
 * This function is **blocking**.
 * It starts a conversion on the sensor ADC for both pressure and temperature, waits until
 * the busy flag clears or for a **10ms timeout**, reads the values from the sensor and converts
 * them to physical quantities.
 *
 * \sa The non-blocking version: #abp2_read_nb().
 *
 * @return                  0 on success
 * @return                  \a -#ETIMEDOUT on timeout
 * @return                  \a -#ENODATA on sensor error
 */
int abp2_read(const abp2_t *dev, int32_t *press, int32_t *temp);

/**
 * @brief Measure pressure and temperature (non-blocking version).
 *
 * @param[in]  dev           Sensor device descriptor.
 * @param[out]  press        Pressure value read from the sensor (thousandths of sensor unit).
 * @param[out]  temp         Temperature read from the sensor (milli-degrees Celsius).
 *
 * This function starts a conversion on the sensor ADC and reads the values
 * from the **previous conversion** in a single bus transfer and converts
 * them to physical quantities.
 *
 * \sa The blocking version: #abp2_read().
 *
 * @return                  0 on success
 * @return                  \a -#ENODATA on sensor error
 */
int abp2_read_nb(const abp2_t *dev, int32_t *press, int32_t *temp);

/**
 * @brief Read the status byte of the sensor.
 *
 * @param[in]  dev          Sensor device descriptor.
 *
 * \sa \ref abp2_statusbits
 *
 * @return The status byte.
 */
uint8_t abp2_getstatus(const abp2_t *dev);

/**
 * @brief   Read pressure and temperature raw values.
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[out]  raw_values  Raw values read from the sensor.
 *
 * Read the raw values, i.e. ADC counts and populate \a raw_values.
 * The raw values can be converted to physical quantities with
 * #abp2_pressure() and #abp2_temperature().
 *
 * \pre A measurement must have been initiated by #abp2_measure()
 * and have completed (ca. 5ms).
 *
 * @return                  0 on success
 * @return                  \a -ENODATA on sensor error.
 */
int abp2_read_raw(const abp2_t *dev, abp2_raw_t *raw_values);

/**
 * @brief   Start measurement and read previous pressure and temperature raw values.
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[out]  raw_values  Raw values read from the sensor.
 *
 * Read the raw values, i.e. ADC counts that result from the previous
 * measurement.
 * Then, populate \a raw_values.
 * The raw values can be converted to physical quantities with
 * #abp2_pressure() and #abp2_temperature().
 *
 * \pre A measurement must have been initiated by #abp2_measure()
 * or a previous call to this function, in order to retrieve valid data.
 *
 * @note This function leads to concise user code. However, in order
 * to get valid data, it must be called periodically, and at short intervals.
 * If the application favors power savings i.e. low frequency measurements, then
 * #abp2_measure() and #abp2_read_raw() are a better option.
 *
 * @return                  0 on success
 * @return                  \a -ENODATA on sensor error.
 */
int abp2_measure_read(const abp2_t *dev, abp2_raw_t *raw_values);

/**
 * @brief Convert ADC counts to pressure.
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[in]  raw_values   Raw values read from the sensor.
 *
 * \sa #abp2_params to know how to set the sensor range.
 * @return The pressure in thousandths of user units, as set in the parameters
 * of #abp2_init().
 */
int32_t abp2_pressure(const abp2_t *dev, const abp2_raw_t *raw_values);

/**
 * @brief Convert ADC counts to temperature .
 *
 * @param[in]  dev          Sensor device descriptor.
 * @param[in]  raw_values   Raw values read from the sensor.
 *
 * @return The temperature in milli-degrees Celsius.
 */
int32_t abp2_temperature(const abp2_t *dev, const abp2_raw_t *raw_values);

#ifdef __cplusplus
}
#endif

#endif /* ABP2_H */
/** @} */
