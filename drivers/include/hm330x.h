/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_hm330x HM330X Laser Particulate Matter Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for HM330X particle matter sensor
 *
 * @{
 *
 * About
 * =====
 *
 * This driver provides an interface for the HM-330/3600 laser dust sensor.
 * The datasheet can be found [here](https://files.seeedstudio.com/wiki/Grove-Laser_PM2.5_Sensor-HM330X/res/HM-3300%263600_V2.1.pdf)
 *
 * The device also support an UART mode, but the currently available
 * breakout from [seedstudio](https://www.seeedstudio.com/Grove-Laser-PM2-5-Sensor-HM330X.html)
 * only supports I2C.
 *
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     HM330X deployment setting
 *
 * The HM330X sensor outputs two set of PM* values, one calibrated for indoor
 * environment and another one for atmospheric environment, set this value
 * according to your deployment.
 *
 */
#ifndef CONFIG_HM330X_INDOOR_ENVIRONMENT
#define CONFIG_HM330X_INDOOR_ENVIRONMENT       1
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
#if IS_USED(MODULE_HM3302)
    uint16_t nc_pm_0p3;     /**< Number concentration of all particles <= 0.3µm [#/cm^3] */
    uint16_t nc_pm_0p5;     /**< Number concentration of all particles <= 0.5µm [#/cm^3] */
    uint16_t nc_pm_1;       /**< Number concentration of all particles <= 1µm [#/cm^3] */
    uint16_t nc_pm_2p5;     /**< Number concentration of all particles <= 2.5µm [#/cm^3] */
    uint16_t nc_pm_5;       /**< Number concentration of all particles <= 5µm [#/cm^3] */
    uint16_t nc_pm_10;      /**< Number concentration of all particles <= 10µm [#/cm^3] */
#endif
} hm330x_data_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;          /**< The I2C device */
    gpio_t reset_pin;   /**< Reset pin, active low */
    gpio_t set_pin;     /**< Set/Enable pin, active high*/
} hm330x_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    hm330x_params_t params;     /**< parameters of the sensor device */
} hm330x_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 *
 * @retval  0         Success
 * @retval -EIO       Failed to initialize GPIO pins
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int hm330x_init(hm330x_t *dev, const hm330x_params_t *params);

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
int hm330x_read(hm330x_t *dev, hm330x_data_t* data);

/**
 * @brief       Reset the sensor.
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm330x_reset(hm330x_t *dev);

/**
 * @brief       Set Device to Sleep
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm330x_sleep(hm330x_t *dev);

/**
 * @brief       Wakeup Device
 *
 * @param[in]   dev        Device descriptor of the driver
 */
void hm330x_wakeup(hm330x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
