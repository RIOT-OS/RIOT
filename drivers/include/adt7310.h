/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_adt7310 ADT7310 SPI temperature sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Analog Devices ADT7310 temperature sensor.
 *
 * ## Description
 *
 * The ADT7310 is a high accuracy digital temperature sensor
 * in a narrow SOIC package. It contains a band gap temperature
 * reference and a 13-bit ADC to monitor and digitize the
 * temperature to a 0.0625°C resolution. The ADC resolution,
 * by default, is set to 13 bits (0.0625 °C). This can be changed
 * to 16 bits (0.0078 °C) by setting Bit 7 in the configuration
 * register (Register Address 0x01).
 * The ADT7310 is guaranteed to operate over supply voltages from
 * 2.7 V to 5.5 V. Operating at 3.3 V, the average supply current is
 * typically 210 μA. The ADT7310 has a shutdown mode that
 * powers down the device and offers a shutdown current of
 * typically 2 μA. The ADT7310 is rated for operation over the
 * −55°C to +150°C temperature range.
 *
 * ## Usage
 *
 * See `tests/drivers/adt7310` for an example application using this driver.
 *
 * ## Caveats
 *
 * This driver is currently missing support for a number of hardware features:
 *
 *  - Interrupt and compare pins are not handled
 *  - There is no public API for setting the temperature alarm levels
 *  - Device SPI reset is not implemented (drive MISO high from the master while clocking SCK)
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the ADT7310 sensor driver.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ADT7310_H
#define ADT7310_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for ADT7310 sensors.
 */
typedef struct {
    spi_t spi;              /**< SPI bus the sensor is connected to */
    spi_clk_t clk;          /**< SPI bus clock speed */
    gpio_t cs;              /**< CS pin GPIO handle */
    bool initialized;       /**< sensor status, true if sensor is initialized */
    bool high_res;          /**< Sensor resolution, true if configured to 16 bit resolution */
} adt7310_t;

/**
 * @name    ADT7310 configuration bits
 * @{
 */
#define ADT7310_CONF_FAULT_QUEUE_MASK  (0x03)
#define ADT7310_CONF_FAULT_QUEUE_SHIFT (0)
#define ADT7310_CONF_FAULT_QUEUE(x) (((x) << ADT7310_CONF_FAULT_QUEUE_SHIFT) & ADT7310_CONF_FAULT_QUEUE_MASK)
#define ADT7310_CONF_CT_POL_MASK  (0x04)
#define ADT7310_CONF_CT_POL_SHIFT (2)
#define ADT7310_CONF_CT_POL(x) (((x) << ADT7310_CONF_CT_POL_SHIFT) & ADT7310_CONF_CT_POL_MASK)
#define ADT7310_CONF_INT_POL_MASK  (0x08)
#define ADT7310_CONF_INT_POL_SHIFT (3)
#define ADT7310_CONF_INT_POL(x) (((x) << ADT7310_CONF_INT_POL_SHIFT) & ADT7310_CONF_INT_POL_MASK)
#define ADT7310_CONF_INTCT_MODE_MASK  (0x10)
#define ADT7310_CONF_INTCT_MODE_SHIFT (4)
#define ADT7310_CONF_INTCT_MODE(x) (((x) << ADT7310_CONF_INTCT_MODE_SHIFT) & ADT7310_CONF_INTCT_MODE_MASK)
#define ADT7310_CONF_OPERATION_MODE_MASK  (0x60)
#define ADT7310_CONF_OPERATION_MODE_SHIFT (5)
#define ADT7310_CONF_OPERATION_MODE(x) (((x) << ADT7310_CONF_OPERATION_MODE_SHIFT) & ADT7310_CONF_OPERATION_MODE_MASK)
#define ADT7310_CONF_RESOLUTION_MASK  (0x80)
#define ADT7310_CONF_RESOLUTION_SHIFT (7)
#define ADT7310_CONF_RESOLUTION(x) (((x) << ADT7310_CONF_RESOLUTION_SHIFT) & ADT7310_CONF_RESOLUTION_MASK)

/**
 * @brief   Continuous operation mode
 */
#define ADT7310_MODE_CONTINUOUS (ADT7310_CONF_OPERATION_MODE(0))
/**
 * @brief   One shot
 */
#define ADT7310_MODE_ONE_SHOT   (ADT7310_CONF_OPERATION_MODE(1))
/**
 * @brief   1 sample per second
 */
#define ADT7310_MODE_1SPS       (ADT7310_CONF_OPERATION_MODE(2))
/**
 * @brief   Shut down (powersave)
 */
#define ADT7310_MODE_SHUTDOWN   (ADT7310_CONF_OPERATION_MODE(3))
/** @} */

/**
 * @brief   Set configuration register of an ADT7310 sensor
 *
 * @param[in]  dev          pointer to sensor device descriptor
 * @param[in]  config       configuration byte, see macros in adt7310.h
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int adt7310_set_config(adt7310_t *dev, uint8_t config);

/**
 * @brief   Initialize the ADT7310 sensor driver.
 *
 * @note The SPI bus is expected to have been initialized when adt7310_init is called.
 *
 * @param[in]  dev          pointer to sensor device descriptor
 * @param[in]  spi          SPI bus the sensor is connected to
 * @param[in]  clk          SPI bus speed
 * @param[in]  cs           GPIO pin the chip select signal is connected to
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int adt7310_init(adt7310_t *dev, spi_t spi, spi_clk_t clk, gpio_t cs);

/**
 * @brief   Read raw temperature register value
 *
 * @note The three least-significant bits of the value register are used for
 *       flags if the sensor is configured for 13 bit mode.
 *
 * @param[in]  dev          pointer to sensor device descriptor
 *
 * @return                  raw sensor value on success
 * @return                  INT16_MIN on error
 */
int16_t adt7310_read_raw(const adt7310_t *dev);

/**
 * @brief   Read temperature value from sensor and convert to milli-degrees Celsius.
 *
 * Divide the returned value by 1000 to get integer degrees.
 *
 * @param[in]  dev          pointer to sensor device descriptor
 *
 * @return                  temperature in milli-degrees Celsius
 * @return                  INT32_MIN on errors
 */
int32_t adt7310_read(const adt7310_t *dev);

/**
 * @brief   Read temperature value from sensor and convert to degrees Celsius.
 *
 * @param[in]  dev          pointer to sensor device descriptor
 *
 * @return                  floating point representation of temperature in degrees Celsius
 * @return                  NaN on errors
 */
float adt7310_read_float(const adt7310_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ADT7310_H */
