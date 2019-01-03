/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bh1750fvi BH1750FVI Light Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Rohm BH1750FVI ambient light sensor
 *
 * @{
 * @file
 * @brief       Interface definition for the Rohm BH1750FVI ambient light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BH1750FVI_H
#define BH1750FVI_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Possible I2C bus addresses of the device
 *
 * The actual address of the device depends on the state of the ADDR pin.
 * @{
 */
#define BH1750FVI_ADDR_PIN_LOW          (0x23)      /**< ADDR pin := 0 */
#define BH1750FVI_ADDR_PIN_HIGH         (0x5c)      /**< ADDR pin := 1 */
/** @} */

/**
 * @brief   Default address of BH1750FVI sensors
 */
#define BH1750FVI_DEFAULT_ADDR          BH1750FVI_ADDR_PIN_LOW

/**
 * @brief   Maximum I2C bus speed to use with the device
 */
#define BH1750FVI_I2C_MAX_CLK           I2C_SPEED_FAST

/**
 * @brief   Status and error return codes
 */
enum {
    BH1750FVI_OK      =  0,     /**< everything was fine */
    BH1750FVI_ERR_I2C = -1      /**< error initializing the I2C bus */
};

/**
 * @brief   Device descriptor for BH1570FVI devices
 */
typedef struct {
    i2c_t i2c;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< slave address of the device */
} bh1750fvi_t;

/**
 * @brief   Set of configuration parameters for BH1750FV devices
 */
typedef struct {
    i2c_t i2c;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< slave address of the device */
} bh1750fvi_params_t;

/**
 * @brief   Initialize the given BH1750FVI device
 *
 * @param[out] dev      device descriptor of the targeted device
 * @param[in] params    device configuration (i2c bus, address and bus clock)
 *
 * @return      0 on success
 * @return      -1 if unable to speak to the device
 */
int bh1750fvi_init(bh1750fvi_t *dev, const bh1750fvi_params_t *params);

/**
 * @brief   Read a ambient light value from the given device [in LUX]
 *
 * The result value is the measured ambient light intensity in LUX and ranges
 * from 0 to 54612. Taking one measurement takes ~120ms, so it takes this amount
 * of time until the function returns.
 *
 * @param[in] dev       device descriptor of the targeted device
 *
 * @return      ambient light intensity in LUX
 */
uint16_t bh1750fvi_sample(const bh1750fvi_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* BH1750FVI_H */
/** @} */
