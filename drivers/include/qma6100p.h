/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-FileCopyrightText: 2026 Léandre Le Duc <leandre.leduc38@gmail.com>
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
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 */

#include <stdbool.h>
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
    QMA6100P_NO_NEW_DATA = 2,  /**< no new data available (normal polling state) */
    QMA6100P_NOI2C = -1,       /**< I2C communication failed */
    QMA6100P_NODEV = -2,       /**< no QMA6100P device found on the bus */
    QMA6100P_INVALID_ARG = -3, /**< invalid argument */
    QMA6100P_GPIO_ERROR = -4,  /**< GPIO error */
    QMA6100P_TIMEOUT = -5,     /**< operation timed out */
};

/**
 * @brief   Full-scale range selection for the QMA6100P accelerometer
 *
 * Values map directly to bits[3:0] of the RANGE register (0x0F)
 * Higher range = lower sensitivity
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
 * Values map directly to bits[4:0] of the ODR register (0x10)
 * Assumes MCLK=51.2kHz (default after init sequence)
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
 * @brief Master Clock selection
 *
 * Master clock is controlled by PM_REGISTER (0x11) and can be set through I2C commands.
 *
 * @note By default, QMA6100P has a 51.2kHz clock rate after init sequence.
 */
typedef enum {
    QMA6100P_PM_MCLK_51K2 = 4, /**< 51.2 kHz Default after the power on */
    QMA6100P_PM_MCLK_25K6 = 5, /**< 25.6 kHz */
    QMA6100P_PM_MCLK_12K8 = 6, /**< 12.8 kHz */
    QMA6100P_PM_MCLK_6K4 = 7,  /**< 6.4 kHz */
} qma6100p_mclk_t;

/**
 * @brief   QMA6100P interrupt callback
 */
typedef void (*qma6100p_int_cb_t)(void *);

/**
 * @brief   INT pin output mode (INTPIN_CONF, 0x20)
 */
typedef enum {
    QMA6100P_INTPIN_PUSH_PULL = 0,  /**< INT pin push-pull mode */
    QMA6100P_INTPIN_OPEN_DRAIN = 1, /**< INT pin open-drain mode */
} qma6100p_int_pin_mode_t;

/**
 * @brief   INT pin active level (INTPIN_CONF, 0x20)
 */
typedef enum {
    QMA6100P_INTPIN_ACTIVE_LOW = 0,  /**< INT pin active LOW on interrupt */
    QMA6100P_INTPIN_ACTIVE_HIGH = 1, /**< INT pin active HIGH on interrupt */
} qma6100p_int_active_level_t;

/**
 * @brief   Data shadowing mode (INT_CFG, 0x21)
 */
typedef enum {
    /** shadowing enabled: lock the MSB content while the LSB is read so both
     * bytes belong to the same sample, preserving acceleration data integrity
     * (default) */
    QMA6100P_INT_CFG_SHADOW_EN = 0,
    /** shadowing disabled: no lock, MSB/LSB may come from different samples */
    QMA6100P_INT_CFG_SHADOW_DIS = 1,
} qma6100p_int_shadow_t;

/**
 * @brief   Interrupt pin number selection (INT1_MAP1 0x1A / INT2_MAP1 0x1C)
 */
typedef enum {
    QMA6100P_INT1 = 1, /**< Use INT1 pin for interrupt output */
    QMA6100P_INT2 = 2, /**< Use INT2 pin for interrupt output */
} qma6100p_int_pin_num_t;

/**
 * @brief   Configuration parameters
 */
typedef struct {
    i2c_t i2c;    /**< I2C bus the device is connected to */
    uint8_t addr; /**< I2C address (@ref QMA6100P_I2C_ADDR_LOW or _HIGH) */
    /** MCU GPIO connected to the QMA6100P INT1 pin (@ref GPIO_UNDEF if unused) */
    gpio_t int1_pin;
    /** MCU GPIO connected to the QMA6100P INT2 pin (@ref GPIO_UNDEF if unused) */
    gpio_t int2_pin;
    qma6100p_odr_t rate;                    /**< output data rate */
    qma6100p_range_t range;                 /**< full-scale range */
    qma6100p_mclk_t mclk;                   /**< master clock */
    qma6100p_int_shadow_t interrupt_shadow; /**< acceleration data shadow mode */
} qma6100p_params_t;

/**
 * @brief   Device descriptor for QMA6100P accelerometer
 */
typedef struct {
    qma6100p_params_t params; /**< Device configuration parameters */
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
 * @brief   Converted acceleration data in µg
 */
typedef struct {
    int32_t x; /**< acceleration in X direction [µg] */
    int32_t y; /**< acceleration in Y direction [µg] */
    int32_t z; /**< acceleration in Z direction [µg] */
} qma6100p_data_t;

/**
 * @brief   Initialize the QMA6100P accelerometer driver
 *
 * Runs the init sequence, sets ODR, range and master clock from @p params.
 *
 * The device is left in active mode; call @ref qma6100p_set_low_power
 * afterwards to enter Ultra-Low Power State (ULPS) if needed.
 *
 * @param[out] dev          device descriptor of accelerometer to initialize
 * @param[in]  params       configuration parameters
 *
 * @retval                  QMA6100P_OK on success
 * @retval                  QMA6100P_NOI2C if initialization of I2C bus failed
 * @retval                  QMA6100P_NODEV if accelerometer test failed
 */
int qma6100p_init(qma6100p_t *dev, const qma6100p_params_t *params);

/**
 * @brief   Enter Ultra-Low Power State (ULPS)
 *
 * @param[in,out] dev        device descriptor of accelerometer
 *
 * @retval                   QMA6100P_OK on success
 * @retval                   QMA6100P_NOI2C if I2C transaction failed
 * @retval                   QMA6100P_NODEV if device not found on bus
 */
int qma6100p_set_low_power(qma6100p_t *dev);

/**
 * @brief   Enter active mode
 *
 * Exits Ultra-Low Power State (ULPS) and restores the configured parameters.
 *
 * @param[in,out] dev        device descriptor of accelerometer
 *
 * @retval                   QMA6100P_OK on success
 * @retval                   QMA6100P_NOI2C if I2C transaction failed
 * @retval                   QMA6100P_NODEV if device not found on bus
 */
int qma6100p_set_active_mode(qma6100p_t *dev);

/**
 * @brief   Read raw accelerometer data (ADC counts)
 *
 * @param[in] dev           device descriptor of accelerometer
 * @param[out] data         raw ADC counts per axis
 *
 * @retval                  QMA6100P_NO_NEW_DATA if nothing has changed and keep data unchanged.
 */
int qma6100p_read_raw(const qma6100p_t *dev, qma6100p_raw_data_t *data);

/**
 * @brief   Read accelerometer data converted to ug
 *
 * Converts raw counts using the configured range sensitivity.
 *
 * @param[in] dev           device descriptor of accelerometer
 * @param[out] data         acceleration in ug per axis
 *
 * @retval                  QMA6100P_NO_NEW_DATA if nothing has changed and keep data unchanged
 */
int qma6100p_read(const qma6100p_t *dev, qma6100p_data_t *data);

/**
 * @brief   Configure data-ready interrupt
 *
 * Writes INTPIN_CONF, INTx_MAP1, and INT_EN1 registers to route the data-ready
 * event to the @p line INT pin, then arms the matching MCU GPIO ISR
 * (@ref qma6100p_params_t::int1_pin / @ref qma6100p_params_t::int2_pin).
 *
 * @param[in,out] dev        device descriptor of accelerometer
 * @param[in] line           INT line to route the data-ready event to
 *                           (@ref QMA6100P_INT1 or @ref QMA6100P_INT2)
 * @param[in] cb             callback invoked when the data-ready event fires
 * @param[in] arg            argument passed to @p cb
 *
 * @retval                   QMA6100P_OK on success
 * @retval                   QMA6100P_GPIO_ERROR if the line's GPIO is unwired or initialization failed
 * @retval                   QMA6100P_INVALID_ARG if line is invalid
 * @retval                   QMA6100P_NOI2C if I2C transaction failed
 * @retval                   QMA6100P_NODEV if device not found on bus
 *
 * @warning The callback is invoked from interrupt context, keep it short
 */
int qma6100p_set_data_ready_int(qma6100p_t *dev, qma6100p_int_pin_num_t line,
                                qma6100p_int_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */
