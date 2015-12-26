/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tmp006 TMP006 Infrared Thermopile Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Texas Instruments TMP006 sensor.
 *              The sensor measures the temperature of an object
 *              without the need of direct contact with the object.
 *              After initialization and set active the sensor
 *              will make measurements at periodic times.
 *              The conversion duration depends on oversample ratio.<br>
 *              The oversample ratio can be determined
 *              by sensor initialization.<br>
 *              The target object temperature can be calculated as follows:<br><br>
 *              Temperature of sensor's die (2 times right shifting and devide-by 32):
 *              \f{eqnarray*}{
 *                  T_{\mathrm{DIE}} &=& \frac{T_{\mathrm{RAW}}}{128}
 *              \f}
 *              Difference Temperature in Kelvin:
 *              \f{eqnarray*}{
 *                  T_{\mathrm{DIFF}} &=& T_{\mathrm{DIE}} - T_{\mathrm{REF}}
 *              \f}
 *              Sensitivity of the thermopile sensor,
 *              with \f$S_{\mathrm{0}}\f$ as calibration factor:
 *              \f{eqnarray*}{
 *                  S &=& S_{\mathrm{0}} \cdot ( 1 + a_1 \cdot T_{\mathrm{DIFF}}
 *                        + a_2 \cdot T_{\mathrm{DIFF}}^2 ) \\
 *              \f}
 *              Sensor's voltage (the LSB size is \f$ 156.25\,\mathrm{nV} \f$):
 *              \f{eqnarray*}{
 *                  V_{\mathrm{OBJ}} &=& V_{\mathrm{RAW}} \cdot 156.25\,\mathrm{nV}
 *              \f}
 *              Offset voltage:
 *              \f{eqnarray*}{
 *                  V_{OS} &=& b_0 + b_1 \cdot T_{\mathrm{DIFF}}
 *                             + b_2 \cdot T_{\mathrm{DIFF}}^2 \\
 *              \f}
 *              Seebeck coefficients of the thermopile:
 *              \f{eqnarray*}{
 *                  f(V_{\mathrm{OBJ}}) &=& (V_{\mathrm{OBJ}} - V_{O\mathrm{S}})
 *                                          + c_2 \cdot (V_{\mathrm{OBJ}} - V_{\mathrm{OS}})^2 \\
 *              \f}
 *              Temperature of the target object:
 *              \f{eqnarray*}{
 *                  T_{\mathrm{OBJ}} &=& \sqrt[4]{T_{\mathrm{DIE}}^4
 *                                       + \frac{f(V_{\mathrm{OBJ}})}{S}} \\
 *              \f}
 *              Constants:<br>
 *              \f{eqnarray*}{
 *                  a_{\mathrm{1}} &=& 1.75 \cdot 10^{-3} \\
 *                  a_{\mathrm{2}} &=& -1.678 \cdot 10^{-5} \\
 *                  T_{\mathrm{REF}} &=& 298.15\,\mathrm{K} \\
 *                  b_{\mathrm{0}} &=& -2.94 \cdot 10^{-5} \\
 *                  b_{\mathrm{1}} &=& -5.7 \cdot 10^{-7} \\
 *                  b_{\mathrm{2}} &=& 4.63 \cdot 10^{-9} \\
 *                  c_{\mathrm{2}} &=& 13.4
 *              \f}
 *
 *              The calculation and constants are wrapped from TI TMP006 User's Guide SBOU107.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the TMP006 sensor driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
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

#ifndef TMP006_I2C_ADDRESS
#define TMP006_I2C_ADDRESS         0x41 /**< TMP006 Sensor Default Address */
#endif

#ifndef TMP006_CONVERSION_TIME
#define TMP006_CONVERSION_TIME     1E6 /**< Default Conversion Time in us */
#endif

#define TMP006_CONFIG_CR_AS1       0x00 /**< Conversion Time 0.25 s, AVG Samples: 1 */
#define TMP006_CONFIG_CR_AS2       0x01 /**< Conversion Time 0.5 s, AVG Samples: 2 */
#define TMP006_CONFIG_CR_AS4       0x02 /**< Conversion Time 1 s, AVG Samples: 4 */
#define TMP006_CONFIG_CR_AS8       0x03 /**< Conversion Time 2 s, AVG Samples: 8 */
#define TMP006_CONFIG_CR_AS16      0x04 /**< Conversion Time 4 s, AVG Samples: 16 */
#define TMP006_CONFIG_CR_DEF       TMP006_CONFIG_CR_AS4 /**< Default for Testing */

#ifndef TMP006_CCONST_S0
#define TMP006_CCONST_S0           6.4E-14    /**< Calibration Factor */
#endif

#define TMP006_CCONST_A1           1.75E-3    /**< Constant \f$a_{\mathrm{1}}\f$ */
#define TMP006_CCONST_A2           -1.678E-5  /**< Constant \f$a_{\mathrm{2}}\f$ */
#define TMP006_CCONST_TREF         298.15     /**< Constant \f$T_{\mathrm{REF}}\f$ */
#define TMP006_CCONST_B0           -2.94E-5   /**< Constant \f$b_{\mathrm{0}}\f$ */
#define TMP006_CCONST_B1           -5.7E-7    /**< Constant \f$b_{\mathrm{1}}\f$ */
#define TMP006_CCONST_B2           4.63E-9    /**< Constant \f$b_{\mathrm{2}}\f$ */
#define TMP006_CCONST_C2           13.4       /**< Constant \f$c_{\mathrm{2}}\f$ */
#define TMP006_CCONST_LSB_SIZE     156.25E-9  /**< Sensor Voltage Register LSB Size */

/**
 * @brief Device descriptor for TMP006 sensors.
 */
typedef struct {
    i2c_t i2c;              /**< I2C device, the sensor is connected to */
    uint8_t addr;           /**< the sensor's slave address on the I2C bus */
    bool initialized;       /**< sensor status, true if sensor is initialized */
} tmp006_t;

/**
 * @brief TMP006 sensor test.
 * This function looks for Device ID of the TMP006 sensor.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_test(tmp006_t *dev);

/**
 * @brief Initialize the TMP006 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  i2c          I2C bus the sensor is connected to
 * @param[in]  address      sensor's I2C slave address
 * @param[in]  conv_rate    number of averaged samples
 *
 * @return                  0 on success
 * @return                  -1 if conv_rate is wrong
 * @return                  -2 if initialization of I2C bus failed
 * @return                  -3 if sensor test failed
 * @return                  -4 if sensor configuration failed
 */
int tmp006_init(tmp006_t *dev, i2c_t i2c, uint8_t address, uint8_t conv_rate);

/**
 * @brief Reset the TMP006 sensor. After that, the sensor should be reinitialized.
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_reset(tmp006_t *dev);

/**
 * @brief Set active mode, this enables periodic measurements.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_set_active(tmp006_t *dev);

/**
 * @brief Set standby mode.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_set_standby(tmp006_t *dev);

/**
 * @brief Read sensor's data.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] rawv         object voltage value
 * @param[out] rawt         raw die temperature
 * @param[out] drdy         data ready, 0 if a conversion is in progress
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp006_read(tmp006_t *dev, int16_t *rawv, int16_t *rawt, uint8_t *drdy);

/**
 * @brief Convert raw sensor values to temperature.
 *
 * @param[in]  rawv         object voltage value
 * @param[in]  rawt         raw die temperature value
 * @param[out] tamb         converted ambient temperature
 * @param[out] tobj         converted object temperature
 */
void tmp006_convert(int16_t rawv, int16_t rawt,  float *tamb, float *tobj);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
