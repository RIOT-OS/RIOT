/*
 * SPDX-FileCopyrightText: 2020 Nalys
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_bh1900nux BH1900NUX Temperature sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Rohm BH1900NUX Temperature sensor
 *
 * @{
 * @file
 * @brief       Interface definition for the  BH1900NUX temperature sensor
 *
 * @author      Wouter Symons <wsymons@nalys-group.com>
 */

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Possible I2C bus addresses of the device
 *
 * The actual address of the device depends on the states of the slave address pins.
 * @{
 */
#define BH1900NUX_ADDR_1          (0x48)        /**< A2:L A1:L A0:L*/
#define BH1900NUX_ADDR_2          (0x49)        /**< A2:L A1:L A0:H*/
#define BH1900NUX_ADDR_3          (0x4a)        /**< A2:L A1:H A0:L*/
#define BH1900NUX_ADDR_4          (0x4b)        /**< A2:L A1:H A0:H*/
#define BH1900NUX_ADDR_5          (0x4c)        /**< A2:H A1:L A0:L*/
#define BH1900NUX_ADDR_6          (0x4d)        /**< A2:H A1:L A0:H*/
#define BH1900NUX_ADDR_7          (0x4e)        /**< A2:H A1:H A0:L*/
#define BH1900NUX_ADDR_8          (0x4f)        /**< A2:H A1:H A0:H*/
/** @} */

/**
 * @name    Temperature register address
 *
 * Address of the temperature resister
 * @{
 */
#define BH1900NUX_REG_ADDR      (0x00)
/** @} */

/**
 * @brief   Default address of BH1900NUX sensors
 */
#define BH1900NUX_DEFAULT_ADDR          BH1900NUX_ADDR_1

/**
 * @brief   Maximum I2C bus speed to use with the device
 */
#define BH1900NUX_I2C_MAX_CLK           I2C_SPEED_FAST

/**
 * @brief   Status and error return codes
 */
enum {
    BH1900NUX_OK        =  0,   /**< everything was fine */
    BH1900NUX_ERR_I2C   = -1    /**< error initializing the I2C bus */
};

/**
 * @brief   Device descriptor for BH1900NUX devices
 */
typedef struct {
    i2c_t i2c;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< slave address of the device */
} bh1900nux_t;

/**
 * @brief   Set of configuration parameters for BH1900NUX devices
 */
typedef struct {
    i2c_t i2c;          /**< I2C bus the device is connected to */
    uint8_t addr;       /**< slave address of the device */
} bh1900nux_params_t;

/**
 * @brief   Initialize the given BH1900NUX device
 *
 * @param[out] dev      device descriptor of the targeted device
 * @param[in] params    device configuration (i2c bus, address and bus clock)
 *
 * @return      0 on success
 * @return      -1 if unable to speak to the device
 */
int bh1900nux_init(bh1900nux_t *dev, const bh1900nux_params_t *params);

/**
 * @brief   Read the temperature measerd by the device [MILICELSIUS].
 *
 * The result value is the measured temperature in MILICELSUIS and ranges
 * from -30 to 95.
 *
 * @param[in] dev       device descriptor of the targeted device
 * @param[out] temp     temperature output
 *
 * @return  BH1900NUX_OK on success
 * @return  BH1900NUX_ERR_I2C on failure
 */
int bh1900nux_read(const bh1900nux_t *dev,  int16_t *temp);

#ifdef __cplusplus
}
#endif

/** @} */
