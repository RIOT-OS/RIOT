/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tmp006 TI TMP006 Infrared Thermopile Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Texas Instruments TMP006 sensor.
 *
 * The TI TMP006 (Infrared Thermopile Contactless Temperature Sensor) measures
 * the temperature of an object without need of direct contact with the object.
 * After initialization the sensor can be set active for periodic measurements.
 * <br> The conversion duration depends on oversample ratio. The oversample
 * ratio can be determined by sensor initialization. The target object
 * temperature can be calculated as follows:<br><br>
 * Temperature of sensor's die (2 times right shifting and devide-by 32):
 *     \f{eqnarray*}{
 *         T_{\mathrm{DIE}} &=& \frac{T_{\mathrm{RAW}}}{128}
 *     \f}
 * Difference Temperature in Kelvin:
 *     \f{eqnarray*}{
 *         T_{\mathrm{DIFF}} &=& T_{\mathrm{DIE}} - T_{\mathrm{REF}}
 *     \f}
 * Sensitivity of the thermopile sensor, with \f$S_{\mathrm{0}}\f$ as
 * calibration factor:
 *      \f{eqnarray*}{
 *          S &=& S_{\mathrm{0}} \cdot ( 1 + a_1 \cdot T_{\mathrm{DIFF}}
 *                + a_2 \cdot T_{\mathrm{DIFF}}^2 ) \\
 *      \f}
 * Sensor's voltage (the LSB size is \f$ 156.25\,\mathrm{nV} \f$):
 *      \f{eqnarray*}{
 *          V_{\mathrm{OBJ}} &=& V_{\mathrm{RAW}} \cdot 156.25\,\mathrm{nV}
 *      \f}
 * Offset voltage:
 *      \f{eqnarray*}{
 *          V_{OS} &=& b_0 + b_1 \cdot T_{\mathrm{DIFF}}
 *                        + b_2 \cdot T_{\mathrm{DIFF}}^2 \\
 *      \f}
 * Seebeck coefficients of the thermopile:
 *      \f{eqnarray*}{
 *          f(V_{\mathrm{OBJ}}) &=& (V_{\mathrm{OBJ}} - V_{O\mathrm{S}})
 *                                  + c_2 \cdot (V_{\mathrm{OBJ}}
 *                                  - V_{\mathrm{OS}})^2 \\
 *      \f}
 * Temperature of the target object:
 *      \f{eqnarray*}{
 *          T_{\mathrm{OBJ}} &=& \sqrt[4]{T_{\mathrm{DIE}}^4
 *                               + \frac{f(V_{\mathrm{OBJ}})}{S}} \\
 *      \f}
 * Constants:<br>
 *      \f{eqnarray*}{
 *          a_{\mathrm{1}} &=& 1.75 \cdot 10^{-3} \\
 *          a_{\mathrm{2}} &=& -1.678 \cdot 10^{-5} \\
 *          T_{\mathrm{REF}} &=& 298.15\,\mathrm{K} \\
 *          b_{\mathrm{0}} &=& -2.94 \cdot 10^{-5} \\
 *          b_{\mathrm{1}} &=& -5.7 \cdot 10^{-7} \\
 *          b_{\mathrm{2}} &=& 4.63 \cdot 10^{-9} \\
 *          c_{\mathrm{2}} &=& 13.4
 *      \f}
 *
 * The calculation and constants are wrapped from TI TMP006 User's Guide SBOU107.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the TMP006 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef TMP006_H
#define TMP006_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   TMP006 Default Address
 */
#ifndef TMP006_I2C_ADDRESS
#define TMP006_I2C_ADDRESS         (0x41)
#endif

/**
 * @brief   Default Conversion Time in us
 */
#ifndef TMP006_CONVERSION_TIME
#define TMP006_CONVERSION_TIME     (1E6)
#endif

/**
 * @brief   Default low power mode
 *
 * If set to 0, the device will be always-on
 * If set to 1, the device will be put in low power mode between measurements.
 * This adds a @c TMP006_CONVERSION_TIME us delay to each measurement call
 * for bringing the device out of standby.
 */
#ifndef TMP006_USE_LOW_POWER
#define TMP006_USE_LOW_POWER (0)
#endif

/**
 * @brief   Default raw value mode
 *
 * If set to 0, measurements will be converted to Celsius.
 * If set to 1, raw adc readings will be returned.
 */
#ifndef TMP006_USE_RAW_VALUES
#define TMP006_USE_RAW_VALUES (0)
#endif

/**
 * @name    Conversion rate and AVG sampling configuration
 * @{
 */
#define TMP006_CONFIG_CR_AS1       (0x00)   /**< Conversion Time 0.25s, AVG Samples: 1 */
#define TMP006_CONFIG_CR_AS2       (0x01)   /**< Conversion Time 0.5s, AVG Samples: 2 */
#define TMP006_CONFIG_CR_AS4       (0x02)   /**< Conversion Time 1s, AVG Samples: 4 */
#define TMP006_CONFIG_CR_AS8       (0x03)   /**< Conversion Time 2s, AVG Samples: 8 */
#define TMP006_CONFIG_CR_AS16      (0x04)   /**< Conversion Time 4s, AVG Samples: 16 */
#define TMP006_CONFIG_CR_DEF       TMP006_CONFIG_CR_AS4 /**< Default for Testing */
/** @} */

/**
 * @name    Constants for TMP006 calibration
 * @{
 */
#ifndef TMP006_CCONST_S0
#define TMP006_CCONST_S0           (6.4E-14)    /**< Calibration Factor */
#endif
#define TMP006_CCONST_A1           (1.75E-3)    /**< Constant \f$a_{\mathrm{1}}\f$ */
#define TMP006_CCONST_A2           (-1.678E-5)  /**< Constant \f$a_{\mathrm{2}}\f$ */
#define TMP006_CCONST_TREF         (298.15)     /**< Constant \f$T_{\mathrm{REF}}\f$ */
#define TMP006_CCONST_B0           (-2.94E-5)   /**< Constant \f$b_{\mathrm{0}}\f$ */
#define TMP006_CCONST_B1           (-5.7E-7)    /**< Constant \f$b_{\mathrm{1}}\f$ */
#define TMP006_CCONST_B2           (4.63E-9)    /**< Constant \f$b_{\mathrm{2}}\f$ */
#define TMP006_CCONST_C2           (13.4)       /**< Constant \f$c_{\mathrm{2}}\f$ */
#define TMP006_CCONST_LSB_SIZE     (156.25E-9)  /**< Sensor Voltage Register LSB Size */
/** @} */

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t   i2c;        /**< I2C device, the sensor is connected to */
    uint8_t addr;       /**< the sensor's slave address on the I2C bus */
    uint8_t rate;       /**< number of averaged samples */
} tmp006_params_t;

/**
 * @brief   Device descriptor for TMP006 sensors.
 */
typedef struct {
    tmp006_params_t p;  /**< Configuration parameters */
} tmp006_t;

/**
 * @brief   TMP006 specific return values
 */
enum {
    TMP006_OK,          /**< Success, no error */
    TMP006_ERROR_BUS,   /**< I2C bus error */
    TMP006_ERROR_DEV,   /**< internal device error */
    TMP006_ERROR_CONF,  /**< invalid device configuration */
    TMP006_ERROR,       /**< general error */
};

/**
 * @brief   Initialize the TMP006 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success
 * @return                  -TMP006_ERROR_BUS on I2C bus error
 * @return                  -TMP006_ERROR_DEV if sensor test failed
 * @return                  -TMP006_ERROR_CONF if sensor configuration failed
 */
int tmp006_init(tmp006_t *dev, const tmp006_params_t *params);

/**
 * @brief   Reset the TMP006 sensor, afterwards it should be reinitialized.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_reset(const tmp006_t *dev);

/**
 * @brief   Set active mode, this enables periodic measurements.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_set_active(const tmp006_t *dev);

/**
 * @brief   Set standby mode.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_set_standby(const tmp006_t *dev);

/**
 * @brief   Read sensor's data.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] rawv         object voltage value
 * @param[out] rawt         raw die temperature
 * @param[out] drdy         data ready, 0 if a conversion is in progress
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_read(const tmp006_t *dev, int16_t *rawv, int16_t *rawt, uint8_t *drdy);

/**
 * @brief   Convert raw sensor values to temperature.
 *
 * @param[in]  rawv         object voltage value
 * @param[in]  rawt         raw die temperature value
 * @param[out] tamb         converted ambient temperature
 * @param[out] tobj         converted object temperature
 */
void tmp006_convert(int16_t rawv, int16_t rawt,  float *tamb, float *tobj);

/**
 * @brief   Convenience function to get ambient and object temperatures in [°C]
 *
 * @note    Temperature scaled by x100 for accuracy and avoid floats
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] ta           converted ambient temperature
 * @param[out] to           converted object temperature
 */
int tmp006_read_temperature(const tmp006_t *dev, int16_t *ta, int16_t *to);

#ifdef __cplusplus
}
#endif

#endif /* TMP006_H */
/** @} */
