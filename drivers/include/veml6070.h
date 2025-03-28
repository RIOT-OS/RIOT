/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef VEML6070_H
#define VEML6070_H

/**
 * @defgroup    drivers_veml6070 VEML6070 UV sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the VEML6070 UV sensor
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Device driver interface for the VEML6070 UV sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Integration times
 */
typedef enum veml6070_integrationtime {
  VEML6070_HALF_T = 0,              /**< 1/2 T integration time */
  VEML6070_1_T,                     /**< 1 T integration time */
  VEML6070_2_T,                     /**< 2 T integration time */
  VEML6070_4_T,                     /**< 4 T integration time */
} veml6070_itime_t;

/**
 * @brief   Status and error return codes
 */
enum {
    VEML6070_OK = 0,                /**< Everything was fine */
    VEML6070_ERR_I2C                /**< Error initializing the I2C bus */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
     i2c_t i2c_dev;                 /**< I2C device which is used */
     veml6070_itime_t itime;        /**< Integration time */
} veml6070_params_t;

/**
 * @brief   Device descriptor for the VEML6070 sensor
 */
typedef struct {
    veml6070_params_t params;       /**< Device parameters */
} veml6070_t;

/**
 * @brief   Initialize the given VEML6070 device
 *
 * @param[out] dev          Initialized device descriptor of VEML6070 device
 * @param[in]  params       The parameters for the VEML6070 device (integration time)
 *
 * @return                  VEML6070_OK on success
 * @return                  VEML6070_ERR_I2C if given I2C is not enabled in board config
 */
int veml6070_init(veml6070_t *dev, const veml6070_params_t * params);

/**
 * @brief   Read UV index from the given VEML6070 device
 *
 * @param[in] dev           Device descriptor of VEML6070 device to read from
 *
 * @return                  UV index
 */
uint16_t veml6070_read_uv(const veml6070_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* VEML6070_H */
