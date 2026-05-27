/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_qma6100p 3-axis QMA6100P Accelerometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the QMA6100P 3-Axis accelerometer.
 *
 * The driver will initialize the accelerometer for best resolution. After the
 * initialization the accelerometer will make measurements at periodic times.
 * The measurements period and scale range can be determined by accelerometer
 * initialization.
 * This driver only implements basic functionality: data-ready interrupt
 * support only (no tap, motion, step, or FIFO interrupts).
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the QMA6100P accelerometer driver.
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Named return values
 */
enum {
    QMA6100P_OK = 0,           /**< everything was fine */
    QMA6100P_DATA_READY = 1,   /**< new data ready to be read */
    QMA6100P_NOI2C = -1,       /**< I2C communication failed */
    QMA6100P_NODEV = -2,       /**< no QMA6100P device found on the bus */
    QMA6100P_NODATA = -3,      /**< no data available */
    QMA6100P_INVALID_ARG = -4, /**< invalid argument */
    QMA6100P_GPIO_ERROR = -5,  /**< GPIO error */
};

/**
 * @brief   Full-scale range selection for the QMA6100P accelerometer
 *
 * Values map directly to bits[3:0] of the RANGE register (0x0F).
 * Higher range = lower sensitivity.
 */
typedef enum {
    QMA6100P_RANGE_2G = 0x01,  /**< ±2g  — sensitivity 4096 LSB/g */
    QMA6100P_RANGE_4G = 0x02,  /**< ±4g  — sensitivity 2048 LSB/g */
    QMA6100P_RANGE_8G = 0x04,  /**< ±8g  — sensitivity 1024 LSB/g */
    QMA6100P_RANGE_16G = 0x08, /**< ±16g — sensitivity  512 LSB/g */
    QMA6100P_RANGE_32G = 0x0F, /**< ±32g — sensitivity  256 LSB/g */
} qma6100p_range_t;

/**
 * @brief   Output data rate selection
 *
 * Values map directly to bits[4:0] of the ODR register (0x10).
 * Assumes MCLK=51.2KHz (default after init sequence).
 */
typedef enum {
    QMA6100P_ODR_100HZ = 0x00,  /**< 100 Hz (default) */
    QMA6100P_ODR_200HZ = 0x01,  /**< 200 Hz */
    QMA6100P_ODR_400HZ = 0x02,  /**< 400 Hz */
    QMA6100P_ODR_800HZ = 0x03,  /**< 800 Hz */
    QMA6100P_ODR_1600HZ = 0x04, /**< 1600 Hz */
    QMA6100P_ODR_50HZ = 0x05,   /**< 50 Hz */
    QMA6100P_ODR_25HZ = 0x06,   /**< 25 Hz */
    QMA6100P_ODR_12HZ5 = 0x07,  /**< 12.5 Hz */
} qma6100p_odr_t;

/**
 * @brief  Mode selection
 *
 * Work mode is controlled by PM_REGISTER (0x11) and can be set through I2C commands.
 *
 * @warning By default, QMA6100P is in intermediate state after power on but it shouldn't be keep for any applications.
 */
typedef enum {
    QMA6100P_MODE_INTERMEDIATE = 0, /**< After power on state, shouldn't be keep */
    QMA6100P_MODE_ACTIVE = 1,       /**< Processes the interrupts and send data to results registers */
    QMA6100P_MODE_ULPS = 2,         /**< Ultra-Low Power State */
} qma6100p_mode_t;
/**
 * @brief   INT pin active level
 */
typedef enum {
    QMA6100P_INT_PIN_ACTIVE_HIGH = 0, /**< INT pin active HIGH on interrupt */
    QMA6100P_INT_PIN_ACTIVE_LOW = 1,  /**< INT pin active LOW on interrupt */
} qma6100p_int_pin_active_level_t;

/**
 * @brief   INT pin output mode
 */
typedef enum {
    QMA6100P_INT_PIN_PUSH_PULL = 0,  /**< INT pin configured as push-pull */
    QMA6100P_INT_PIN_OPEN_DRAIN = 1, /**< INT pin configured as open drain */
} qma6100p_int_pin_mode_t;

/**
 * @brief   INT latch mode
 */
typedef enum {
    QMA6100P_INT_NON_LATCH = 0, /**< INT pulse clears automatically */
    QMA6100P_INT_LATCH = 1,     /**< INT held until acknowledged via @ref qma6100p_ack_int */
} qma6100p_int_latch_t;

/**
 * @brief   INT status clear behavior
 */
typedef enum {
    QMA6100P_INT_CLR_ON_ANY_READ = 0, /**< INT_STATUS bits cleared on any read */
    QMA6100P_INT_CLR_ON_LATCH = 1,    /**< INT_STATUS bits cleared only if latched */
} qma6100p_int_clear_t;

/**
 * @brief   QMA6100P interrupt callback
 */
typedef void (*qma6100p_int_cb_t)(void *);

/**
 * @brief   Interrupt configuration parameters
 */
typedef struct {
    gpio_t interrupt_pin;                             /**< GPIO connected to INT pin */
    qma6100p_int_pin_active_level_t active_level_int; /**< active level of INT pin */
    qma6100p_int_pin_mode_t pin_mode_int;             /**< push-pull or open drain for INT */
    qma6100p_int_latch_t interrupt_latch;             /**< interrupt pulse behavior of INT */
    qma6100p_int_clear_t interrupt_clear_behavior;    /**< interrupt status clear behavior */
} qma6100p_int_params_t;

/**
 * @brief   Interrupt descriptor
 */
typedef struct {
    qma6100p_int_params_t params; /**< interrupt configuration */
    qma6100p_int_cb_t cb;         /**< callback function */
    void *arg;                    /**< callback argument */
} qma6100p_int_t;

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus the device is connected to */
    uint8_t addr;           /**< I2C address (@ref QMA6100P_I2C_ADDR_AD0_LOW or _HIGH) */
    qma6100p_odr_t rate;    /**< output data rate */
    qma6100p_range_t range; /**< full-scale range */
    uint8_t offset[3];      /**< user offset correction for X, Y, Z [applied at init] */
    qma6100p_mode_t mode;   /**< operating mode */
} qma6100p_params_t;

/**
 * @brief   Device descriptor for QMA6100P accelerometer
 */
typedef struct {
    qma6100p_params_t params; /**< device configuration parameters */
} qma6100p_t;

/**
 * @brief   Raw 14-bit acceleration data (ADC counts)
 */
typedef struct {
    int16_t x; /**< raw ADC counts in X direction */
    int16_t y; /**< raw ADC counts in Y direction */
    int16_t z; /**< raw ADC counts in Z direction */
} qma6100p_raw_data_t;

/**
 * @brief   Converted acceleration data in g
 */
typedef struct {
    float x; /**< acceleration in X direction [g] */
    float y; /**< acceleration in Y direction [g] */
    float z; /**< acceleration in Z direction [g] */
} qma6100p_data_t;
/**
 * @brief   Initialize the QMA6100P accelerometer driver.
 *
 * Applies offset correction from @p params, sets ODR and range, then
 * puts the device into active mode.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  QMA6100P_OK on success
 * @return                  QMA6100P_NOI2C if initialization of I2C bus failed
 * @return                  QMA6100P_NODEV if accelerometer test failed
 */
int qma6100p_init(qma6100p_t *dev, const qma6100p_params_t *params);

/**
 * @brief   Set operating mode.
 *
 * @param[in,out]  dev       device descriptor of accelerometer
 * @param[in]      mode      mode to set (@ref QMA6100P_MODE_ACTIVE or @ref QMA6100P_MODE_INTERMEDIATE)
 *
 * @return                   QMA6100P_OK on success
 * @return                   QMA6100P_NOI2C if I2C transaction failed
 * @return                   QMA6100P_NODEV if device not found on bus
 */
int qma6100p_set_mode(qma6100p_t *dev, qma6100p_mode_t mode);

/**
 * @brief   Set standby mode.
 *
 * @param[in]  dev          device descriptor of accelerometer
 */
void qma6100p_set_standby(const qma6100p_t *dev);

/**
 * @brief   Check for new set of measurement data
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @return                  QMA6100P_DATA_READY if new sample is ready
 * @return                  QMA6100P_NODATA if nothing is available
 */
int qma6100p_is_ready(const qma6100p_t *dev);

/**
 * @brief   Read raw accelerometer data (ADC counts)
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         raw ADC counts per axis
 */
void qma6100p_read_raw(const qma6100p_t *dev, qma6100p_raw_data_t *data);

/**
 * @brief   Read accelerometer data converted to g
 *
 * Converts raw counts using the configured range sensitivity.
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         acceleration in g per axis
 */
void qma6100p_read(const qma6100p_t *dev, qma6100p_data_t *data);
/**
 * @brief   Configure data-ready interrupt
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[in]  int_params   interrupt configuration parameters
 * @param[in]  cb           callback invoked on interrupt
 * @param[in]  arg          argument passed to callback
 *
 * @return                  QMA6100P_OK on success
 * @return                  QMA6100P_GPIO_ERROR if GPIO initialization failed
 */
int qma6100p_set_int(const qma6100p_t *dev, const qma6100p_int_params_t *int_params,
                     qma6100p_int_cb_t cb, void *arg);

/**
 * @brief   Acknowledge interrupt
 *
 * Clears the interrupt status register. Do not call from within an ISR
 * as this performs an I2C transaction.
 *
 * @param[in]  dev          device descriptor of accelerometer
 */
void qma6100p_ack_int(const qma6100p_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
