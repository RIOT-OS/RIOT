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

#include <stdint.h>

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
 * Assumes MCLK=51.2KHz (default after init sequence)
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
 * Work mode is controlled by PM_REGISTER (0x11) and can be set through I2C commands
 *
 * @warning By default, QMA6100P is in intermediate state after power on but it shouldn't be keep for any applications
 */
typedef enum {
    QMA6100P_MODE_INTERMEDIATE = 0, /**< After power on state, shouldn't be keep */
    QMA6100P_MODE_ACTIVE = 1,       /**< Processes the interrupts and send data to results registers */
    QMA6100P_MODE_ULPS = 2,         /**< Ultra-Low Power State */
} qma6100p_mode_t;

/**
 * @brief Master Clock selection
 *
 * Master clock is controlled by PM_REGISTER (0x11) and can be set through I2C commands.
 *
 * @note By default, QMA6100P has a 51K2 after init sequence.
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
 * @brief   INT latch mode (INT_CFG, 0x21)
 */
typedef enum {
    QMA6100P_INT_CFG_NON_LATCH = 0, /**< INT pulse clears automatically */
    QMA6100P_INT_CFG_LATCH = 1,     /**< INT held until ack via @ref qma6100p_ack_int */
} qma6100p_int_latch_t;

/**
 * @brief   INT_STATUS clear behavior (INT_CFG, 0x21)
 */
typedef enum {
    QMA6100P_INT_CFG_CLR_ON_LATCH = 0,    /**< INT_STATUS bits cleared only if latched */
    QMA6100P_INT_CFG_CLR_ON_ANY_READ = 1, /**< INT_STATUS bits cleared on any read */
} qma6100p_int_clr_t;

/**
 * @brief   Data shadowing mode (INT_CFG, 0x21)
 */
typedef enum {
    QMA6100P_INT_CFG_SHADOW_EN = 0,  /**< shadowing enabled (default) */
    QMA6100P_INT_CFG_SHADOW_DIS = 1, /**< shadowing disabled */
} qma6100p_int_shadow_t;

/**
 * @brief   Interrupt pin number selection (INT1_MAP1 0x1A / INT2_MAP1 0x1C)
 */
typedef enum {
    QMA6100P_INT1 = 1, /**< Use INT1 pin for interrupt output */
    QMA6100P_INT2 = 2, /**< Use INT2 pin for interrupt output */
} qma6100p_int_pin_num_t;

/**
 * @brief   Interrupt configuration parameters
 */
typedef struct {
    gpio_t interrupt_pin;                         /**< MCU GPIO connected to the QMA6100P INT pin */
    qma6100p_int_active_level_t active_level_int; /**< active level of INT pin */
    qma6100p_int_pin_mode_t pin_mode_int;         /**< INT pin output mode */
    qma6100p_int_latch_t interrupt_latch;         /**< latch mode */
    qma6100p_int_clr_t interrupt_clear_behavior;  /**< status clear mode */
    qma6100p_int_shadow_t interrupt_shadow;       /**< shadow mode */
    qma6100p_int_pin_num_t interrupt_pin_num;     /**< QMA6100P INT pin routed on the board */
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
    uint8_t addr;           /**< I2C address (@ref QMA6100P_I2C_ADDR_LOW or _HIGH) */
    qma6100p_odr_t rate;    /**< output data rate */
    qma6100p_range_t range; /**< full-scale range */
    qma6100p_mclk_t mclk;   /**< master clock */
    uint8_t offset[3];      /**< user offset correction for X, Y, Z [applied at init] */
    qma6100p_mode_t mode;   /**< operating mode */
} qma6100p_params_t;

/**
 * @brief   Device descriptor for QMA6100P accelerometer
 */
typedef struct {
    qma6100p_params_t params; /**< Device configuration parameters */
    qma6100p_int_t interrupt; /**< Device interrupt descriptor */
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
    int32_t x; /**< acceleration in X direction [ug] */
    int32_t y; /**< acceleration in Y direction [ug] */
    int32_t z; /**< acceleration in Z direction [ug] */
} qma6100p_data_t;

/**
 * @brief   Initialize the QMA6100P accelerometer driver
 *
 * Applies offset correction from @p params, sets ODR and range, then
 * puts the device into active mode
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
 * @brief   Set operating mode
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
 * @brief   Read raw accelerometer data (ADC counts)
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         raw ADC counts per axis
 *
 * @return                  QMA6100P_NODATA if nothing has changed and keep data unchanged.
 */
int qma6100p_read_raw(const qma6100p_t *dev, qma6100p_raw_data_t *data);

/**
 * @brief   Read accelerometer data converted to ug
 *
 * Converts raw counts using the configured range sensitivity
 *
 * @param[in]  dev          device descriptor of accelerometer
 * @param[out] data         acceleration in ug per axis
 *
 * @return                  QMA6100P_NODATA if nothing has changed and keep data unchanged
 */
int qma6100p_read(const qma6100p_t *dev, qma6100p_data_t *data);

/**
 * @brief   Configure data-ready interrupt
 *
 * Writes INTPIN_CONF, INT1/2_MAP1, and INT_EN1 registers to route the
 * data-ready event to the selected INT pin, then arms the MCU GPIO ISR
 *
 * @param[in,out] dev        device descriptor of accelerometer
 * @param[in]     interrupt  interrupt descriptor (@ref qma6100p_int_t): pin,
 *                           polarity, output mode, latch, routing, callback
 *
 * @return                   QMA6100P_OK on success
 * @return                   QMA6100P_GPIO_ERROR if GPIO initialization failed
 * @return                   QMA6100P_INVALID_ARG if interrupt_pin_num is invalid
 * @return                   QMA6100P_NOI2C if I2C transaction failed
 * @return                   QMA6100P_NODEV if device not found on bus
 *
 * @warning The callback is invoked from interrupt context, keep it short
 */
int qma6100p_set_data_ready_int(qma6100p_t *dev, const qma6100p_int_t *interrupt);

/**
 * @brief   Acknowledge interrupt
 *
 * Clears the interrupt status register
 *
 * @param[in]  dev          device descriptor of accelerometer
 *
 * @warning Do not call from within an ISR as this performs an I2C transaction
 */
void qma6100p_ack_int(const qma6100p_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
