/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef HSC_H
#define HSC_H

/**
 * @defgroup    drivers_hsc HSC temperature and pressure sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the HSC sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the HSC sensor.
 *
 * @author      Quang Pham <phhr_quang@live.com>
 */

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                   /**< I2C device which is used */
    uint8_t i2c_addr;                /**< I2C address */
    uint8_t hsc_range;               /**< Pressure range in mBar */
} hsc_params_t;

/**
 * @brief   Device descriptor for the HSC sensor
 */
typedef struct {
    hsc_params_t params;             /**< Device initialization parameters */
} hsc_t;

/**
 * @brief   Initialize the given HSC device
 *
 * @param[out] dev          Initialized device descriptor of HSC device
 * @param[in]  params       Initialization parameters
 *
 * @retval 0                Success
 * @retval -ENXIO           No HSC at given address
 * @return -EIO             An error occurred when reading calibration values
 */
int hsc_init(hsc_t *dev, const hsc_params_t *params);

/**
 * @brief   Read temperature value from the given HSC device, returned in 0.1°C
 *
 * @param[in]  dev          Device descriptor of HSC device to read from
 * @param[out] dest         Temperature in 0.1°C
 *
 * @retval 0                Success
 * @retval -ENXIO           No HSC at given address
 * @return -EIO             An error occurred when reading calibration values
 */
int hsc_read_temperature(const hsc_t *dev, int16_t *dest);

/**
 * @brief   Read pressure value from the given HSC device, returned in uBar
 *
 * @param[in]  dev          Device descriptor of HSC device to read from
 * @param[out] dest         Pressure in uBar
 *
 * @retval 0                Success
 * @retval -ENXIO           No HSC at given address
 * @return -EIO             An error occurred when reading calibration values
 */
int hsc_read_pressure(const hsc_t *dev, int32_t *dest);

/**
 * @name    Default configuration values
 * @{
 */
#if !defined(CONFIG_HSC_I2C_ADDR) || defined(DOXYGEN)
#define CONFIG_HSC_I2C_ADDR         (0x28)      /**< Use address 0x28 by default */
#endif
#if !defined(CONFIG_HSC_RANGE) || defined(DOXYGEN)
#define CONFIG_HSC_RANGE            (40U)       /**< Default to 40 millibar range */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HSC_H */
