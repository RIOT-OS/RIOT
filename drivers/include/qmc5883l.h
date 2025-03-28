/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef QMC5883L_H
#define QMC5883L_H

/**
 * @defgroup    drivers_qmc5883l QMC5883L 3-Axis Digital Magnetic Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for QST QMC5883L digital magnetic sensor
 *
 * # About
 * This module provides a device driver for the QML5883L magnetic sensor
 * (digital compass) by QST. This device is a successor and similar to the
 * Honeywell HMC5883L, it does use a different register map and uses a different
 * style of configuration.
 *
 * # Usage
 * Use the qmc5883l_init() function to initialize your sensor. On exit of the
 * initialization function, the sensor is put into continuous sampling mode
 * (power on) mode.
 *
 * For reading the sampled data, you have two options: use polling or use
 * interrupt based notifications.
 *
 * ## Polling
 * You call periodically call qmc5883l_read[_raw]() directly and simply check
 * the return value to be QMC5883L_OK or QMC5883L_OVERFLOW for valid data.
 * Alternatively call qmc5883l_data_ready() to explicitly ask the sensor if new
 * data is available.
 *
 * ## Interrupt based
 * For the interrupt mode to be available, you have to build the driver with the
 * associated functions using `USEMODULE += qmc5883l_int`.
 *
 * To configure and enable interrupt notifications for data ready events use the
 * qmc5883l_init_int() function. This will setup the configured interrupt pin
 * (params->pin_drdy) and enable the DRDY pin output for the QMC5883L sensor.
 *
 * @warning     The DRDY interrupt callback is executed in interrupt context, so
 *              **do not** call any driver API function directly inside the
 *              callback! Instead use some IPC to notify a thread.
 *
 * @{
 *
 * @file
 * @brief       API definition for the QMC5883L device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

/**
 * @brief   Static QMC5883L I2C address (can not be changed)
 */
#define QMC5883L_ADDR           (0x0d)

/**
 * @brief   Error codes used by the QMC5883L driver
 */
enum {
    QMC5883L_OK,            /**< success */
    QMC5883L_NODATA,        /**< no data are available */
    QMC5883L_OVERFLOW,      /**< at least one axis overflowed its range */
    QMC5883L_BUSERR,        /**< i2c bus error */
    QMC5883L_NOCFG,         /**< configuration error */
};

/**
 * @brief   Output data rate
 *
 * "Output data rate is controlled by ODR registers. Four data update
 * frequencies can be selected: 10Hz, 50Hz, 100Hz and 200Hz. For most of
 * compassing applications, we recommend 10 Hz for low power consumption. For
 * gaming, the high update rate such as 100Hz or 200Hz can be used."
 * (datasheet V1.0, p.17)
 */
typedef enum {
    QMC5883L_ODR_10HZ  = (0u << 2),     /**< 10Hz data output rate */
    QMC5883L_ODR_50HZ  = (1u << 2),     /**< 50Hz data output rate */
    QMC5883L_ODR_100HZ = (2u << 2),     /**< 100Hz data output rate */
    QMC5883L_ODR_200HZ = (3u << 2),     /**< 200Hz data output rate */
} qmc5883l_odr_t;

/**
 * @brief   Output value range
 *
 * "Field ranges of the magnetic sensor can be selected through the register
 * RNG. The full scale field range is determined by the application
 * environments. For magnetic clear environment, low field range such as
 * +/- 2gauss can be used. The field range goes hand in hand with the
 * sensitivity of the magnetic sensor. The lowest field range has the highest
 * sensitivity, therefore, higher resolution." (datasheet V1.0, p.17)
 */
typedef enum {
    QMC5883L_RNG_2G = (0u << 4),        /**< 2 Gauss data output range */
    QMC5883L_RNG_8G = (1u << 4),        /**< 8 Gauss data output range */
} qmc5883l_rng_t;

/**
 * @brief   Over sample rate (bandwidth of internal digital filter)
 *
 * "Over sample Rate (OSR) registers are used to control bandwidth of an
  * internal digital filter. Larger OSR value leads to smaller filter bandwidth,
  * less in-band noise and higher power consumption. It could be used to reach a
  * good balance between noise and power. Four over sample ratio can be
  * selected, 64, 128, 256 or 512." (datasheet V1.0, p.17)
 */
typedef enum {
    QMC5883L_OSR_512 = (0u << 6),       /**< 512 samples per reading */
    QMC5883L_OSR_256 = (1u << 6),       /**< 256 samples per reading */
    QMC5883L_OSR_128 = (2u << 6),       /**< 128 samples per reading */
    QMC5883L_OSR_64  = (3u << 6),       /**< 64 samples per reading */
} qmc5883l_osr_t;

/**
 * @brief   QMC5883L initialization parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus the sensor is connected to */
    gpio_t pin_drdy;        /**< DRDY ISR pin, set to GPIO_UNDEF if unused */
    qmc5883l_odr_t odr;     /**< output data rate */
    qmc5883l_rng_t rng;     /**< output data range */
    qmc5883l_osr_t osr;     /**< oversampling rate */
} qmc5883l_params_t;

/**
 * @brief   QMC5883L device descriptor
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus the sensor is connected to */
    gpio_t pin_drdy;        /**< DRDY interrupt pin */
    uint8_t cfg;            /**< actual applied device configuration */
} qmc5883l_t;

/**
 * @brief   Initialize the given QMC5883L magnetic sensor
 *
 * @pre     dev != NULL
 * @pre     params != NULL
 *
 * @param[out] dev      QMC5883L device descriptor
 * @param[in] params    configuration parameters
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_NOCFG on configuration error
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_init(qmc5883l_t *dev, const qmc5883l_params_t *params);

/**
 * @brief   Check if new data is available
 *
 * @pre     dev != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 *
 * @return  QMC5883L_OK if new data is available
 * @return  QMC5883L_NODATA if no new data is available
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_data_ready(const qmc5883l_t *dev);

/**
 * @brief   Read sampled data from the device [converted to milli-Gauss]
 *
 * @pre     dev != NULL
 * @pre     data_out != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 * @param[out] data_out buffer for holding the resulting vector, **must** be
                        able to hold 3 data items (x, y, z)
 *
 * @return  QMC5883L_OK on data being written to @p data_out
 * @return  QMC5883L_OVERFLOW on data successfully read, but at least one data
            item overflowed its data range
 * @return  QMC5883L_NODATA if no new data is available, nothing is written to
            @p data_out
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_read(const qmc5883l_t *dev, int16_t *data_out);

/**
 * @brief   Read RAW data from the device
 *
 * This function returns the configured data range of 2 or 8 Gauss mapped to
 * 16-bit signed integers [--32768:32767].
 *
 * @pre     dev != NULL
 * @pre     data_out != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 * @param[out] data_out buffer for holding the resulting vector, **must** be
                        able to hold 3 data items (x, y, z)
 *
 * @return  QMC5883L_OK on data being written to @p data_out
 * @return  QMC5883L_OVERFLOW on data successfully read, but at least one data
            item overflowed its data range
 * @return  QMC5883L_NODATA if no new data is available, nothing is written to
            @p data_out
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_read_raw(const qmc5883l_t *dev, int16_t *data_out);

/**
 * @brief   Power on the sensor (put it into continuous sampling mode)
 *
 * @pre     dev != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_poweron(const qmc5883l_t *dev);

/**
 * @brief   Power off the sensor (put it into standby mode)
 *
 * @pre     dev != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_poweroff(const qmc5883l_t *dev);

#if defined(MODULE_QMC5883L_INT) || defined(DOXYGEN)
/**
 * @brief   Initialize data ready (DRDY) interrupt notifications
 *
 * After this function is called the DRDY interrupt is enabled, so there is no
 * need to call qmc5883l_irq_enable() afterwards.
 */

/**
 * @brief   Configure and enable the data ready (DRDY) interrupt
 *
 * This function sets up the configured GPIO pin to trigger the given callback
 * for rising edges and it enables the interrupt signal generation for the
 * given QMC5883L sensor.
 *
 * @warning The given callback function is executed in interrupt context. Make
            sure not to call any driver API function in that context!
 *
 * @pre     dev != NULL
 * @pre     cb != NULL
 *
 * @param[in] dev       QMC583L device descriptor
 * @param[in] cb        callback function triggered on DRDY events
 * @param[in] arg       optional user argument passed to @p cb
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_NOCFG on GPIO configuration errors
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_init_int(const qmc5883l_t *dev, gpio_cb_t cb, void *arg);

/**
 * @brief   Enable the data ready (DRDY) interrupt
 *
 * @note    Call this function only after you have configured the DRDY interrupt
 *
 * @pre     dev != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_irq_enable(const qmc5883l_t *dev);

/**
 * @brief   Disable the data ready (DRDY) interrupt
 *
 * @pre     dev != NULL
 *
 * @param[in] dev       QMC5883L device descriptor
 *
 * @return  QMC5883L_OK on success
 * @return  QMC5883L_BUSERR on any I2C bus error
 */
int qmc5883l_irq_disable(const qmc5883l_t *dev);
#endif /* defined(MODULE_QMC5883L_INT) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* QMC5883L_H */
