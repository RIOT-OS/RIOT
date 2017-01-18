/*
 * Copyright (C) 2017 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_jc42 JC42 compliant temperature sensor driver
 * @ingroup     drivers_sensors
 *
 * @brief       JC42 compliant temperature sensor driver
 *
 * ## Description
 *
 * The connection between the MCU and jc42 compliant temperature sensors is
 * based on a I2C-interface. There are multiple jc42 compliant temperature
 * sensors available such as the mcp9804 and the MAX6604. This driver reads the
 * temperature from these sensors. The sensors support a alarm wire, but this
 * is not supported by this driver.
 *
 * @{
 *
 * @file
 * @brief       Driver for jc42 compliant temperature sensors
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef JC42_TEMP_H
#define JC42_TEMP_H

#include "periph/i2c.h"
#include "saul.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name jc42 status return codes
 * @{
 */
#define JC42_OK                       (0)
#define JC42_NOI2C                    (-1)
#define JC42_NODEV                    (-2)
/** @} */


/**
  * @brief   Device descriptor for a jc42 device
  */
typedef struct {
    i2c_t i2c;          /**< I2C device that sensor is connected to */
    uint8_t addr;       /**< I2C address of this particular sensor */
} jc42_t;

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c;	        /**< I2C device that sensor is connected to */
    i2c_speed_t speed;  /**< I2C device speed */
    uint8_t addr;       /**< Configured address of the sensor */
} jc42_params_t;

/**
 * @brief export SAUL endpoint
 */
extern const saul_driver_t jc42_temperature_saul_driver;

/**
 * @brief   Initialize a jc42 device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       jc42 initialization struct
 *
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int jc42_init(jc42_t* dev, jc42_params_t* params);

/**
 * @brief   Get content of configuration register
 *
 * @param[in] dev           device descriptor
 * @param[out] data         buffer where config register will be written to
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int jc42_get_config(jc42_t* dev, uint16_t* data);

/**
 * @brief   Set content of configuration register
 *
 * @param[in] dev           device descriptor
 * @param[in] data          new value for configuration register
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int jc42_set_config(jc42_t* dev, uint16_t data);


/**
 * @brief   Get measured temperature
 *
 * @param[in] dev           device descriptor
 * @param[out] temperature  uint16_t buffer where temperature will be written to in centi-degree
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int jc42_get_temperature(jc42_t* dev, int16_t* temperature);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* JC42_TEMP_H */
