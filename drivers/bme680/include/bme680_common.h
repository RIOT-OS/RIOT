/*
 * Copyright (C) 2019 Mesotic SAS
 *               2020 Gunar Schorcht
 *               2020 OVGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_BME680 BME680
 * @ingroup     drivers_sensors
 * @brief       Driver for the Bosch BME680 sensor.
 *
 * @{
 *
 * @file
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>

 */

#ifndef BME680_COMMON_H
#define BME680_COMMON_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the common API for SPI and I2C for the driver */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uint16_t gas_heating_time;   /**< heating time of gas sensor */
    uint16_t gas_heating_temp;   /**< target heating temperature of gas sensor */
    uint8_t ambient_temp;        /**< ambient temperature */
    uint8_t temp_os;             /**< oversampling setting of temperature sensor */
    uint8_t press_os;            /**< oversampling setting of pressure sensor */
    uint8_t hum_os;              /**< oversampling setting of humidity sensor */
    uint8_t filter;              /**< IIR filter for short time fluctuations in temperature and pressure */
    bool meas_gas;               /**< disable/enable gas measurement */
} bme680_common_params_t;

/**
 * @brief   Device calibration data
 */
typedef struct {
    uint16_t par_t1;
    int16_t par_t2;
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t par_t3;
    uint16_t par_p1;
    int16_t par_p2;
    int16_t par_p4;
    int16_t par_p5;
    int16_t par_p8;
    int16_t par_p9;
    int16_t par_g2;
    int8_t par_h3;
    int8_t par_h4;
    int8_t par_h5;
    uint8_t par_h6;
    int8_t par_h7;
    int8_t par_p3;
    int8_t par_p6;
    int8_t par_p7;
    uint8_t par_p10;
    int8_t par_g1;
    int8_t par_g3;
    uint8_t res_heat_val;
    uint8_t res_heat_range;
    uint8_t range_sw_error;
} bme680_calib_t;

/**
 * @brief   Oversampling modes for temperature, pressure, humidity
 */
enum {
    OVERSAMPLING_1 = 0b001,
    OVERSAMPLING_2 = 0b010,
    OVERSAMPLING_4 = 0b011,
    OVERSAMPLING_8 = 0b100,
    OVERSAMPLING_16 = 0b101
};

/**
 * @brief   Filter coefficients
 */
enum {
    FILTER_COEFFICIENT_0 = 0b000,
    FILTER_COEFFICIENT_1 = 0b001,
    FILTER_COEFFICIENT_3 = 0b010,
    FILTER_COEFFICIENT_7 = 0b011,
    FILTER_COEFFICIENT_15 = 0b100,
    FILTER_COEFFICIENT_31 = 0b101,
    FILTER_COEFFICIENT_63 = 0b110,
    FILTER_COEFFICIENT_127 = 0b111
};

/**
 * @brief   Result data
 */
typedef struct {
    uint32_t temperature;           /**< temperature in degree Celsius x 100 */
    uint32_t humidity;              /**< humidity in % */
    uint32_t pressure;              /**< pressure in Pascal */
    uint32_t gas_resistance;        /**< gas resistance in Ohm */
    uint32_t t_fine;                /**< temperature value used for pressure calculation */
    uint8_t gas_status;             /**< gas status indicating success (1) or error (0) */
} bme680_data_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    bme680_common_params_t params;
    /** Device calibration data */
    bme680_calib_t calib;
} bme680_common_t;

/**
 * @brief   Converts heating temperature to register value
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   res        resulting regiser value
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 */
int convert_res_heat(const bme680_common_t *dev, uint8_t *res);
/**
 * @brief   Calculation of temperature value
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   res        calculated temperature value in percent
 * @param[inout] t_fine     calculated temperature parameter
 * @param[in]    temp_adc   Register value of temperature measurement
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 * @return -ERANGE          Error in calculating result data
 */
int calc_temp(const bme680_common_t *dev, uint32_t *res, uint32_t *t_fine, uint32_t temp_adc);

/**
 * @brief   Calculation of humidity value
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   res        calculated humidity value in percent
 * @param[inout] temp_com   compensated temperature value
 * @param[in]    hum_adc    Register value of humidity measurement
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 * @return -ERANGE          Error in calculating result data
 */
int calc_hum(const bme680_common_t *dev, uint32_t *res, const uint32_t *temp_comp, const uint16_t hum_adc);

/**
 * @brief   Calculation of pressure value
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   res        calculated pressure value in Pascal
 * @param[inout] t_fine     calculated temperature parameter
 * @param[in]    press_adc  Register value of pressure measurement
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 * @return -ERANGE          Error in calculating result data
 */
int calc_press(const bme680_common_t *dev, uint32_t *res, uint32_t *t_fine, const uint32_t press_adc);

/**
 * @brief   Calculation of gas value
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   res        calculated gas value in Ohm
 * @param[in]    gas_range  Register value of gas range
 * @param[in]    gas_adc    Register value of gas measurement
 *
 * @return                  0 on success
 * @return -EIO             Error in reading/writing data
 * @return -ERANGE          Error in calculating result data
 */
void calc_gas(const bme680_common_t *dev, uint32_t *res, uint8_t gas_range, uint16_t gas_adc);

/**
 * @brief   Calculation of heating duration register value
 *
 * @param[inout] dur        Target heating duration
 * @param[out]   res        register value of heating duration
 */
void calc_heater_dur(const uint16_t dur, uint8_t *res);

#ifdef __cplusplus
}
#endif

#endif /* BME680_COMMON_H */
/** @} */
