/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_tmp117
 * @ingroup     drivers_sensors
 * @brief       Device driver for TMP117 temperature sensor
 *
 * @{
 * @file
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 */

#include "periph/i2c.h"
#include <errno.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Named return values
 */
enum {
    TMP117_OK           =  0,       /**< everything was fine */
    TMP117_DATAREADY    =  1,       /**< data is ready to be read */
    TMP117_NOI2C        = -EIO,     /**< I2C communication failed */
    TMP117_NODEV        = -ENODEV,  /**< no TMP117 device found on the bus */
    TMP117_NODATA       = -ENODATA  /**< no data available */
};

typedef enum {
    TMP117_CONV_CC  = 0,    /**< continous conversion mode */
    TMP117_CONV_SD  = 1,    /**< shutdown conversion mode */
    TMP117_CONV_OS  = 3     /**< one shot conversion mode */
} tmp117_conv_mode_t;

typedef enum {
    TMP117_CONV_CYCLE_15_MS     = 0,    /**< 15.5ms conversion cycle */
    TMP117_CONV_CYCLE_125_MS    = 1,    /**< 125ms conversion cycle */
    TMP117_CONV_CYCLE_250_MS    = 2,    /**< 250ms conversion cycle */
    TMP117_CONV_CYCLE_500_MS    = 3,    /**< 500ms conversion cycle */
    TMP117_CONV_CYCLE_1_S       = 4,    /**< 1s conversion cycle */
    TMP117_CONV_CYCLE_4_S       = 5,    /**< 4s conversion cycle */
    TMP117_CONV_CYCLE_8_S       = 6,    /**< 8s conversion cycle */
    TMP117_CONV_CYCLE_16_S      = 7,    /**< 16s conversion cycle */
} tmp117_conv_cycle_t;

typedef enum {
    TMP117_NO_AVG   = 0,    /**< no averaging */
    TMP117_AVG_8    = 1,    /**< 8 Averaged conversions */
    TMP117_AVG_32   = 2,    /**< 32 Averaged conversions */
    TMP117_AVG_64   = 3,    /**< 64 Averaged conversions */
} tmp117_avg_t;

typedef struct {
    i2c_t i2c;
    uint8_t addr;
    tmp117_conv_mode_t conv_mode;   /**< conversion mode */
    tmp117_conv_cycle_t conv_cycle; /**< conversion cycle time */
    tmp117_avg_t avg;               /**< data averaging*/
}tmp117_params_t;

typedef struct {
    tmp117_params_t params;
    bool is_initialized;
} tmp117_t;


/**
 * @brief   Initializes a TMP117 device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @retval TMP117_OK on success
 * @retval TMP117_NODEV if no device is found on the bus
 * @retval TMP117_NOI2C if other error occurs
 */
int tmp117_init(tmp117_t *dev, const tmp117_params_t *params);

/**
 * @brief Reads the temperature from the sensor.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in centi-celsius
 *
 * @return TMP117_OK on success
 * @return TMP117_I2C if other error occurs
 */
int tmp117_read_temperature(tmp117_t *dev, int16_t *value);

/**
 * @brief Update the conversion mode of the sensor.
 *
 * @param[in, out] dev device descriptor
 * @param[in] mode conversion mode value
 *
 * @return TMP117_OK on success
 * @return TMP117_I2C if other error occurs
 */
int tmp117_set_conversion_mode(tmp117_t *dev, tmp117_conv_mode_t mode);

/**
 * @brief Update the conversion cycle of the sensor.
 *
 * @param[in, out] dev device descriptor
 * @param[in] cycle conversion cycle value
 *
 * @return TMP117_OK on success
 * @return TMP117_I2C if other error occurs
 */
int tmp117_set_conversion_cycle(tmp117_t *dev, tmp117_conv_cycle_t cycle);

/**
 * @brief Update the averaging value of the sensor.
 *
 * @param[in, out] dev device descriptor
 * @param[in] avg averaging value
 *
 * @return TMP117_OK on success
 * @return TMP117_I2C if other error occurs
 */
int tmp117_set_averaging(tmp117_t *dev, tmp117_avg_t avg);

/**
 * @brief Check if the sensor have a new temperature measure available.
 *
 * @param[in, out] dev device descriptor
 *
 * @return TMP117_DATAREADY a new data is available
 * @return TMP117_OK no new data available
 * @return TMP117_I2C if other error occurs
 */
int tmp117_is_data_ready(tmp117_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
