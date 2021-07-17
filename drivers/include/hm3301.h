/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hm3301 HM3301 Laser Particulate Matter Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for HM3301 particle matter sensor
 *
 * @{
 *
 * About
 * =====
 *
 * This driver provides an interface for the HM-330/3600 laser dust sensor.
 * The datasheet can be found [here](https://files.seeedstudio.com/wiki/Grove-Laser_PM2.5_Sensor-HM3301/res/HM-3300%263600_V2.1.pdf)
 *
 * The device also support an UART mode, but the currently available
 * breakout from [seedstudio](https://www.seeedstudio.com/Grove-Laser-PM2-5-Sensor-HM3301.html)
 * only supports I2C.
 *
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef HM3301_H
#define HM3301_H

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     HM3301 deployment setting
 *
 * The HM3301 sensor outputs two set of PM* values, one calibrated for indoor
 * environment and another one for atmospheric environment, set this value
 * according to your deployment.
 *
 */
#ifndef CONFIG_HM3301_INDOOR_ENVIRONMENT
#define CONFIG_HM3301_INDOOR_ENVIRONMENT       1
#endif

/**
 * @brief     Set of measured particulate matter values as sent by the device
 *
 */
typedef struct {
    uint16_t mc_pm_1;       /**< PM1.0 ug/m3 (ultrafine particles) <= 1µm [µg/m^3] */
    uint16_t mc_pm_2p5;     /**< PM2.5 ug/m3 (combustion particles, organic compounds, metals) <= 2.5µm [µg/m^3] */
    uint16_t mc_pm_10;      /**< PM10 ug/m3  (dust, pollen, mould spores) <= 10µm [µg/m^3] */
    uint16_t amc_pm_1;      /**< PM1.0 ug/m3 (atmospheric environment) <= 1µm [µg/m^3] */
    uint16_t amc_pm_2p5;    /**< PM2.5 ug/m3 (atmospheric environment) <= 2.5µm [µg/m^3] */
    uint16_t amc_pm_10;     /**< PM10 ug/m3  (atmospheric environment) <= 10µm [µg/m^3] */
    uint16_t nc_pm_0p3;     /**< Number concentration of all particles <= 0.3µm [#/cm^3] */
    uint16_t nc_pm_0p5;     /**< Number concentration of all particles <= 0.5µm [#/cm^3] */
    uint16_t nc_pm_1;       /**< Number concentration of all particles <= 1µm [#/cm^3] */
    uint16_t nc_pm_2p5;     /**< Number concentration of all particles <= 2.5µm [#/cm^3] */
    uint16_t nc_pm_5;       /**< Number concentration of all particles <= 5µm [#/cm^3] */
    uint16_t nc_pm_10;      /**< Number concentration of all particles <= 10µm [#/cm^3] */
} hm3301_data_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;          /**< The I2C device */
    gpio_t reset_pin;   /**< Reset pin, active low */
    gpio_t set_pin;     /**< Set/Enable pin, active high*/
} hm3301_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    hm3301_params_t params;     /**< parameters of the sensor device */
} hm3301_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @retval  0         Success
 * @retval -EIO       Failed to initialize GPIO pins
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int hm3301_init(hm3301_t *dev, const hm3301_params_t *params);

/**
 * @brief   Read particle matter measurements
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  data       Pre-allocated particle matter data
 *
 * @retval  0         Success
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int hm3301_read(hm3301_t *dev, hm3301_data_t* data);

/**
 * @brief       Reset the sensor.
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm3301_reset(hm3301_t *dev);

/**
 * @brief       Set Device to Sleep
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm3301_sleep(hm3301_t *dev);

/**
 * @brief       Wakeup Device
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm3301_wakeup(hm3301_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* HM3301_H */
/** @} */
