/*
 * Copyright (C) 2015-2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_adxl362 ADXL362 Accelerometer
 * @ingroup     drivers_sensors
 * @brief       Device driver for Analog Devices ADXL362 Micropower, 3-Axis,
 *              ±2 g/±4 g/±8 g Digital Output MEMS Accelerometer
 * @{
 *
 * @file
 * @brief       Device driver for Analog Devices ADXL362 Micropower, 3-Axis,
 *              ±2 g/±4 g/±8 g Digital Output MEMS Accelerometer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ADXL362_H_
#define ADXL362_H_

#include <stdint.h>
#include <stdbool.h>

#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the internal FIFO buffer
 */
#define ADXL362_FIFO_SIZE                  (512)
/**
 * @brief Default watermark level when initializing the FIFO.
 *
 * same as power on default
 */
#define ADXL362_FIFO_WATERMARK_DEFAULT     (0x80)

/**
 * @brief Device descriptor for ADXL362 sensors
 */
typedef struct {
    spi_t spi;            /**< SPI bus the sensor is connected to */
    gpio_t cs;            /**< Chip select line used */
    uint8_t scale_shift;  /**< Current scale setting, in number of bit shifts */
} adxl362_t;

/**
 * @brief Result vector for accelerometer measurement
 */
typedef struct
{
    int16_t acc_x;          /**< Acceleration in the X direction in milli-G */
    int16_t acc_y;          /**< Acceleration in the Y direction in milli-G */
    int16_t acc_z;          /**< Acceleration in the Z direction in milli-G */
} adxl362_data_t;

/**
 * @brief Output data rate valid parameter values
 */
typedef enum {
    ADXL362_ODR_12_5HZ = 0x00, /**< 12.5 Hz output data rate */
    ADXL362_ODR_25HZ   = 0x01, /**< 25 Hz output data rate */
    ADXL362_ODR_50HZ   = 0x02, /**< 50 Hz output data rate */
    ADXL362_ODR_100HZ  = 0x03, /**< 100 Hz output data rate */
    ADXL362_ODR_200HZ  = 0x04, /**< 200 Hz output data rate */
    ADXL362_ODR_400HZ  = 0x05, /**< 400 Hz output data rate */
} adxl362_odr_t;

/**
 * @brief Range valid parameter values
 */
typedef enum {
    ADXL362_RANGE_2G   = 0x00, /**< range = +/- 2 g */
    ADXL362_RANGE_4G   = 0x01, /**< range = +/- 4 g */
    ADXL362_RANGE_8G   = 0x02, /**< range = +/- 8 g */
} adxl362_range_t;

/**
 * @brief FIFO mode valid parameter values
 */
typedef enum {
    ADXL362_FIFO_MODE_DISABLED  = 0x00, /**< FIFO disabled */
    ADXL362_FIFO_MODE_FIRST_N   = 0x01, /**< Keep old values, discard new if FIFO is full */
    ADXL362_FIFO_MODE_LAST_N    = 0x02, /**< Discard old values if FIFO is full, keep new values */
    ADXL362_FIFO_MODE_TRIGGERED = 0x03, /**< Event triggered, see data sheet */
} adxl362_fifo_mode_t;

/**
 * @brief Auto-init parameters for ADXL362 sensors
 */
typedef struct {
    spi_t spi;             /**< SPI bus the sensor is connected to */
    gpio_t cs;             /**< Chip select line used */
    adxl362_range_t range; /**< Range setting */
    adxl362_odr_t odr;     /**< ODR setting */
} adxl362_params_t;

/**
 * @brief Initialize a sensor
 *
 * @param[in]  dev      device descriptor of sensor to initialize
 * @param[in]  spi      SPI bus the sensor is connected to
 * @param[in]  cs       GPIO pin the chip select signal is connected to
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_init(adxl362_t *dev, spi_t spi, gpio_t cs);

/**
 * @brief Read acceleration data from FIFO on the device
 *
 * @param[in]  dev      Sensor device descriptor
 * @param[out] acc      Pointer to output buffer for acceleration data
 * @param[in]  count    available space in output buffer, in number of elements
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_read_acc_fifo(const adxl362_t *dev, adxl362_data_t *acc, size_t count);

/**
 * @brief Read the latest acceleration data from the device
 *
 * This bypasses the FIFO and reads straight from the DATA_XYZ registers on the
 * device.
 *
 * @param[in]  dev      Sensor device descriptor
 * @param[out] acc      Pointer to output buffer for acceleration data
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_read_acc_now(const adxl362_t *dev, adxl362_data_t *acc);

/**
 * @brief Read temperature data from the device, in milli-Celsius
 *
 * @param[in]  dev      Sensor device descriptor
 * @param[out] temp     Pointer to output buffer for temperature data
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_read_temp(const adxl362_t *dev, int32_t *temp);

/**
 * @brief Configure filter parameters in the device
 *
 * See data sheet for details on how to configure the sensor parameters for your application.
 *
 * @param[in]  dev      Sensor device descriptor
 * @param[in]  range    Sensitivity
 * @param[in]  half_bw  true: Use ODR / 4 as the filter frequency for antialiasing filter,
 *                      false: Use ODR / 2 as the filter frequency for antialiasing filter.
 * @param[in]  odr      Output data rate, the rate of new samples added to the FIFO
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_set_filter(adxl362_t *dev, adxl362_range_t range, adxl362_odr_t odr, bool half_bw);

/**
 * @brief Configure FIFO parameters in the device
 *
 * See data sheet for details on how to configure the FIFO for your application.
 *
 * @param[in]  dev         Sensor device descriptor
 * @param[in]  store_temp  Store temperature data along with X,Y,Z acceleration in FIFO (default: no)
 * @param[in]  mode        FIFO discard mode (first N, last N)
 * @param[in]  watermark   FIFO watermark level (max 511)
 *
 * @return 0 on success
 * @return <0 on error
 */
int adxl362_set_fifo(const adxl362_t *dev, bool store_temp, adxl362_fifo_mode_t mode, unsigned int watermark);

/**
 * @brief Perform device self test
 *
 * Vendor provided algorithm
 *
 * @param[in]  dev         Sensor device descriptor
 *
 * @return 1 on self-test passed
 * @return 0 on self-test failed
 * @return <0 on error
 */
int adxl362_self_test(const adxl362_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* ADXL362_H_ */
/** @} */
