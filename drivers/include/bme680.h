/*
 * Copyright (C) 2020 OVGU Magdeburg
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
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#ifndef BME680_H
#define BME680_H

#include "periph/i2c.h"
#include "periph/spi.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   BME680 I2C parameters
 */
typedef struct {
    i2c_t dev;                    /**< I2C device which is used */
    uint8_t addr;                 /**< I2C address */
} bme680_intf_i2c_t;

/**
 * @brief   BME680 SPI parameters
 */
typedef struct {
    spi_t dev;                  /**< SPI device which is used */
    gpio_t nss_pin;             /**< Chip Select pin */
} bme680_intf_spi_t;

/**
 * @brief   BME680 Hardware interface parameters union
 */
typedef union {
    bme680_intf_i2c_t i2c;        /**< I2C specific interface parameters */
    bme680_intf_spi_t spi;        /**< SPI specific interface parameters */
} bme680_intf_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uint8_t ambient_temp;
    uint8_t temp_os;
    uint8_t press_os;
    uint8_t hum_os;
    bool meas_gas;
    uint16_t gas_heating_time;
    uint16_t gas_heating_temp;
    uint8_t filter;
    bme680_intf_t intf;
} bme680_params_t;

/**
 * @brief   Device calibration data
 */
typedef struct {
    uint16_t par_t1;
    int16_t par_t2;
    int8_t par_t3;
    uint16_t par_h1;
    uint16_t par_h2;
    int8_t par_h3;
    int8_t par_h4;
    int8_t par_h5;
    uint8_t par_h6;
    int8_t par_h7; 
    uint16_t par_p1;
    int16_t par_p2;
    int8_t par_p3;
    int16_t par_p4;
    int16_t par_p5;
    int8_t par_p6;
    int8_t par_p7;
    int16_t par_p8;
    int16_t par_p9;
    uint8_t par_p10;
    int8_t par_g1;
    int16_t par_g2;
    int8_t par_g3;
    int8_t res_heat_range;
} bme680_calib_t;

/**
 * @brief   Oversampling modes
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
    uint32_t humidity;              /**< humidity in %  */
    uint32_t pressure;              /**< pressure in Pascal */
    uint8_t gas_status;             /**< gas status indicating success (1) or error (0) */
    uint32_t gas_resistance;        /**< gas resistance in Ohm */
    uint32_t t_fine;                /**< temperature value used for pressure calculation */
} bme680_data_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    bme680_params_t params;
    bme680_calib_t calib;
} bme680_t;

/**
 * @brief   References to BME680 sensor devices
 */
extern bme680_t *bme680_devs[];

/**
 * @brief   Number of initialized BME680 sensor devices in bme680_devs
 */
extern unsigned int bme680_devs_numof;

/**
 * @brief   Status and error return codes
 */
enum {
    BME680_OK = 0,                              /**< everything was fine */
    BME680_ERR_NODEV = -1,                      /**< did not detect BME680 */
    BME680_ERR_INTF = -2,                       /**< error when accessing I2C/SPI bus */   
    BME680_ERR_CALC_TEMP = -3,                  /**< error when calculating temperature */
    BME680_ERR_CALC_HUM = -4,                   /**< error when calculating humidity */ 
    BME680_ERR_CALC_PRESS = -5,                 /**< error when calculating pressure */
    BME680_ERR_CALC_GAS = -6,                   /**< error when calculating gas */ 
};

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[in]    params     Initialization parameters
 *
 * @return                  BME680_OK on success
 * @return                  BME680_ERR_NODEV if device could not be accessed
 * @return                  BME680_ERR_INTF if data could not be read
 */
int bme680_init(bme680_t *dev, const bme680_params_t *params);

/**
 * @brief   Reads temperature, humidity, pressure and gas. Reading gas can be skipped by setting meas_gas = 0
 *
 * @param[inout] dev        Initialized device descriptor of BME680 device
 * @param[out]   data       read temperature (Celsius), humidity (%), pressure (Pa) and gas (Ohm)    
 *
 * @return                  BME680_OK on success
 * @return                  BME680_ERR_INTF if data could not be read
 * @return                  BME680_ERR_CALC_TEMP if temperature could not be calculated
 * @return                  BME680_ERR_CALC_HUM if humidity could not be calculated
 * @return                  BME680_ERR_CALC_PRESS if pressure could not be calculated
 * @return                  BME680_ERR_CALC_GAS if gas resistance could not be calculated         
 */
uint16_t bme680_read(const bme680_t *dev, bme680_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* BME680_H */
/** @} */
