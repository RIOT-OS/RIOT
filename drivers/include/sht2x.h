/*
 * Copyright (C) 2016,2017,2018 Kees Bakker, SODAQ
 * Copyright (C) 2017 George Psimenos
 * Copyright (C) 2018 Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SHT2X_H
#define SHT2X_H

/**
 * @defgroup    drivers_sht2x SHT2x Humidity and Temperature sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the SHT2x sensor
 * @{
 *
 * @file
 * @brief       Device driver implementation for the SHT2x humidity and
 *              temperature humidity sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      George Psimenos <gp7g14@soton.ac.uk>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Status and error return codes
 */
enum {
    SHT2X_OK            =  0,           /**< everything was fine */
    SHT2X_ERR_I2C       = -1,           /**< error initializing the I2C bus */
    SHT2X_ERR_NODEV     = -2,           /**< did not detect SHT2x */
    SHT2X_ERR_NOCAL     = -3,           /**< could not read calibration data */
    SHT2X_ERR_I2C_READ  = -4,           /**< I2C read error */
    SHT2X_ERR_TIMEDOUT  = -5,           /**< timed out */
    SHT2X_ERR_CRC       = -6,           /**< CRC error */
    SHT2X_ERR_USERREG   = -7,           /**< cannot write User Reg */
    SHT2X_ERR_RES       = -8,           /**< invalid resolution */
    SHT2X_ERR_OTHER     = -999,         /**< fatal error */
};

/**
 * @brief       Available resolutions
 *
 * @details     The values represent bits 7 and 0 in the User Register
 */
typedef enum {
    SHT2X_RES_12_14BIT  = 0x00,         /**< RH=12bit, T=14bit */
    SHT2X_RES_8_12BIT   = 0x01,         /**< RH= 8bit, T=12bit */
    SHT2X_RES_10_13BIT  = 0x80,         /**< RH=10bit, T=13bit */
    SHT2X_RES_11_11BIT  = 0x81,         /**< RH=11bit, T=11bit */
} sht2x_res_t;

/**
 * @brief       Available Measuring modes
 */
typedef enum {
    SHT2X_MEASURE_MODE_HOLD = 0,        /**< trigger measurement, hold master */
    SHT2X_MEASURE_MODE_NO_HOLD = 1      /**< trigger measurement, no hold master (i.e. poll) */
} sht2x_measure_mode_t;

/**
 * @brief       User register masks
 * @details     Notice that the values of @c sht2x_res_t must only have the
 *              bits in @c SHT2X_USER_RESOLUTION_MASK.
 * @{
 */
#define SHT2X_USER_RESOLUTION_MASK          0x81
#define SHT2X_USER_EOB_MASK                 0x40
#define SHT2X_USER_HEATER_MASK              0x04
#define SHT2X_USER_RESERVED_MASK            0x38
#define SHT2X_USER_OTP_MASK                 0x02
/** @} */

/**
 * @brief       Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                      /**< I2C device               */
    uint8_t i2c_addr;                   /**< I2C address              */
    sht2x_res_t resolution;             /**< resolution bits RH/temp  */
    sht2x_measure_mode_t measure_mode;  /**< measurement mode used    */
    bool is_crc_enabled;                /**< do CRC or not            */
} sht2x_params_t;

/**
 * @brief       Device descriptor for the SHT2X sensor
 */
typedef struct {
    sht2x_params_t params;              /**< Device Parameters */
} sht2x_t;

/**
 * @brief       Initialize the given SHT2X device
 *
 * @param[out]  dev         Initialized device descriptor of SHT2X device
 * @param[in]   params      The parameters for the SHT2x device
 *
 * @return                  SHT2X_OK on success
 * @return                  SHT2X_ERR_I2C if given I2C is not enabled in board config
 * @return                  SHT2X_ERR_RES invalid resolution
 * @return                  SHT2X_ERR_USERREG error writing User Register
 * @return                  errors returned by sht2x_write_userreg or sht2x_read_userreg
 */
int sht2x_init(sht2x_t* dev, const sht2x_params_t* params);

/**
 * @brief       Reset the SHT2X device
 *
 * @param[in]   dev         Device descriptor of SHT2X device to read from
 *
 * @return                  SHT2X_OK on success
 * @return                  SHT2X_ERR_I2C if given I2C is not enabled in board config
 */
int sht2x_reset(sht2x_t* dev);

/**
 * @brief       Read temperature value from the given SHT2X device, returned in
 *              centi °C
 *
 * @details     Notice that this function will sleep (max 85 ms) when the
 *              @a measure_mode is set to @a SHT2X_MEASURE_MODE_NO_HOLD.
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 *
 * @returns                 The temperature in centi Celsius. In case of an error
 *                          it returns INT16_MIN.
 */
int16_t sht2x_read_temperature(const sht2x_t* dev);

/**
 * @brief       Read humidity value from the given SHT2X device, returned in
 *              centi %RH
 *
 * @details     Notice that this function will sleep (max 29 ms) when the
 *              @a measure_mode is set to @a SHT2X_MEASURE_MODE_NO_HOLD.
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 *
 * @returns                 Humidity in centi %RH (i.e. the percentage times 100).
 *                          In case of an error it returns 0 (i.e. an unrealistic value).
 */
uint16_t sht2x_read_humidity(const sht2x_t *dev);

/**
 * @brief       Read identification code from the given SHT2X device
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 * @param[out] buffer       Buffer to write the identification code to
 * @param[in]  buflen       The size of @p buffer
 *
 * @return                  >= 0 on success, number of bytes read
 * @return                  SHT2X_ERR_CRC in case of a CRC mismatch
 * @return                  <0 errors returned by i2c_read_regs
 */
int sht2x_read_ident(const sht2x_t *dev, uint8_t *buffer, size_t buflen);

/**
 * @brief       Read User Register from the given SHT2X device
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 * @param[out] userreg      The value of User Register
 *
 * @return                  SHT2X_OK on success
 * @return                  SHT2X_ERR_I2C if given I2C is not enabled in board config
 * @return                  SHT2X_ERR_OTHER if NULL pointer was given for @p buffer
 */
int sht2x_read_userreg(const sht2x_t *dev, uint8_t *userreg);

/**
 * @brief       Write User Register to the given SHT2X device
 *
 * @param[in]  dev          Device descriptor of SHT2X device to read from
 * @param[in]  userreg      The value of User Register
 *
 * @return                  SHT2X_OK on success
 * @return                  SHT2X_ERR_I2C if given I2C is not enabled in board config
 */
int sht2x_write_userreg(const sht2x_t *dev, uint8_t userreg);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SHT2X_H */
