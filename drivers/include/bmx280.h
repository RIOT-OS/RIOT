/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *               2017 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BMX280_H
#define BMX280_H

/**
 * @defgroup    drivers_bmx280 BMP280/BME280 temperature, pressure and humidity sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the Bosch BMP280 and BME280 sensors
 *
 * BMP280 and BME280 measure temperature in centi °C and pressure in Pa. BME280
 * can also measure relative humidity in %.
 *
 * For more information, see the datasheets:
 * * [BMP280](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001.pdf)
 * * [BME280](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf)
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * ## Usage
 *
 * To include this driver to your application, simply add one of the following
 * to the application's Makefile:
 *
 * ```make
 * # BME280 connected via SPI
 * USEMODULE += bme280_spi
 * # BME280 connected via I2C
 * USEMODULE += bme280_i2c
 * # BMP280 via SPI
 * USEMODULE += bmp280_spi
 * # BMP280 via I2C
 * USEMODULE += bmp280_i2c
 *
 * # When using I2C, specify the default I2C device to use,
 * # and the BME280's address (see datasheet).
 * # The values below are the defaults:
 * CFLAGS += -DBMX280_PARAM_I2C_DEV=I2C_DEV\(0\)
 * CFLAGS += -DBMX280_PARAM_I2C_ADDR=0x77
 *
 * # Using SPI, you can override the default configuration by specifying the
 * # used SPI bus and the ship select pin:
 * CFLAGS += -DBMX280_PARAM_SPI_DEV=SPI_DEV\(x\)
 * CFLAGS += -DBMX280_PARAM_CS=GPIO_PIN\(y,z\)
 * ```
 *
 * This way the default parameters in `drivers/bmx280/include/bmx280_params.h`
 * are replaced by these new values.
 *
 * @{
 * @file
 * @brief       Device driver interface for the BMP280 and BME280 sensors
 *
 * @details     There are three sensor values that can be read: temperature,
 *              pressure and humidity.  The BME280 device usually measures them
 *              all at once.  It is possible to skip measuring either of the
 *              values by changing the oversampling settings.  The driver is
 *              written in such a way that a measurement is only started from
 *              the function that reads the temperature.  In other words, you
 *              always have to call bmx280_read_temperature, and then optionally
 *              you can call the other two.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>
#include "saul.h"

#if defined(MODULE_BME280_SPI) || defined(MODULE_BMP280_SPI)
#define BMX280_USE_SPI
#include "periph/spi.h"
#else
#include "periph/i2c.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Select the number or raw data bytes depending on the device type
 */
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
#define BMX280_RAW_LEN      (8U)
#else
#define BMX280_RAW_LEN      (6U)
#endif

/**
 * @brief   Calibration struct for the BMX280 sensor
 *
 * This must be read from the device at startup.
 */
typedef struct {
    uint16_t dig_T1;    /**< T1 coefficient */
    int16_t dig_T2;     /**< T2 coefficient */
    int16_t dig_T3;     /**< T3 coefficient */

    uint16_t dig_P1;    /**< P1 coefficient */
    int16_t dig_P2;     /**< P2 coefficient */
    int16_t dig_P3;     /**< P3 coefficient */
    int16_t dig_P4;     /**< P4 coefficient */
    int16_t dig_P5;     /**< P5 coefficient */
    int16_t dig_P6;     /**< P6 coefficient */
    int16_t dig_P7;     /**< P7 coefficient */
    int16_t dig_P8;     /**< P8 coefficient */
    int16_t dig_P9;     /**< P9 coefficient */

    uint8_t dig_H1;     /**< H1 coefficient */
    int16_t dig_H2;     /**< H2 coefficient */
    uint8_t dig_H3;     /**< H3 coefficient */
    int16_t dig_H4;     /**< H4 coefficient */
    int16_t dig_H5;     /**< H5 coefficient */
    int8_t dig_H6;      /**< H6 coefficient */
} bmx280_calibration_t;

/**
 * @brief   Values for t_sb field of the BMX280 config register
 */
typedef enum {
    BMX280_SB_0_5  = 0x00,
    BMX280_SB_62_5 = 0x20,
    BMX280_SB_125  = 0x40,
    BMX280_SB_250  = 0x60,
    BMX280_SB_500  = 0x80,
    BMX280_SB_1000 = 0xa0,
    BMX280_SB_10   = 0xc0,
    BMX280_SB_20   = 0xe0,
} bmx280_t_sb_t;

/**
 * @brief   Values for filter field of the BMX280 config register
 */
typedef enum {
    BMX280_FILTER_OFF = 0x00,
    BMX280_FILTER_2   = 0x04,
    BMX280_FILTER_4   = 0x08,
    BMX280_FILTER_8   = 0x0c,
    BMX280_FILTER_16  = 0x10,
} bmx280_filter_t;

/**
 * @brief   Values for mode field of the BMX280 ctrl_meas register
 */
typedef enum {
    BMX280_MODE_SLEEP  = 0x00,
    BMX280_MODE_FORCED = 0x01,
    BMX280_MODE_NORMAL = 0x03,
} bmx280_mode_t;

/**
 * @brief   Values for oversampling settings
 *
 * These values are used for:
 *  - osrs_h field of the BME280 ctrl_hum register
 *  - osrs_t field of the BMX280 ctrl_meas register
 *  - osrs_p field of the BMX280 ctrl_meas register
 */
typedef enum {
    BMX280_OSRS_SKIPPED = 0x00,
    BMX280_OSRS_X1      = 0x01,
    BMX280_OSRS_X2      = 0x02,
    BMX280_OSRS_X4      = 0x03,
    BMX280_OSRS_X8      = 0x04,
    BMX280_OSRS_X16     = 0x05,
} bmx280_osrs_t;

/**
 * @brief   Parameters for the BMX280 sensor
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
#ifdef BMX280_USE_SPI
    /* SPI configuration */
    spi_t spi;                          /**< SPI bus */
    spi_clk_t clk;                      /**< clock speed for the SPI bus */
    gpio_t cs;                          /**< chip select pin */
#else
    /* I2C details */
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
#endif

    /* Config Register */
    bmx280_t_sb_t t_sb;                 /**< standby */
    bmx280_filter_t filter;             /**< filter coefficient */

    /* ctrl_meas */
    bmx280_mode_t run_mode;             /**< ctrl_meas mode */
    bmx280_osrs_t temp_oversample;      /**< ctrl_meas osrs_t */
    bmx280_osrs_t press_oversample;     /**< ctrl_meas osrs_p */

    /* ctrl_hum */
    bmx280_osrs_t humid_oversample;     /**< ctrl_hum osrs_h */
} bmx280_params_t;

/**
 * @brief   Device descriptor for the BMX280 sensor
 */
typedef struct {
    bmx280_params_t params;             /**< Device Parameters */
    bmx280_calibration_t calibration;   /**< Calibration Data */
    int32_t t_fine;                     /**< temperature compensation value */
    uint8_t last_reading[BMX280_RAW_LEN];   /**< cache the RAW data */
} bmx280_t;

/**
 * @brief   Status and error return codes
 */
enum {
    BMX280_OK           =  0,     /**< everything was fine */
    BMX280_ERR_BUS      = -1,     /**< bus error */
    BMX280_ERR_NODEV    = -2,     /**< did not detect BME280 or BMP280 */
};

/**
 * @brief   Export of SAUL interface for temperature sensor
 */
extern const saul_driver_t bmx280_temperature_saul_driver;

/**
 * @brief   Export of SAUL interface for pressure sensor
 */
extern const saul_driver_t bmx280_pressure_saul_driver;

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
/**
 * @brief   Export of SAUL interface for humidity sensor
 */
extern const saul_driver_t bme280_relative_humidity_saul_driver;
#endif

/**
 * @brief   Initialize the given BMX280 device
 *
 * @param[out] dev      device descriptor of the given BMX280 device
 * @param[in]  params   static configuration parameters
 *
 * @return  BMX280_OK on success
 * @return  BMX280_ERR_BUS on bus error
 * @return  BMX280_ERR_NODEV if no corresponding device was found on the bus
 */
int bmx280_init(bmx280_t* dev, const bmx280_params_t* params);

/**
 * @brief   Read temperature value from the given BMX280 device
 *
 * The measured temperature is returned in centi °C (x.xx°C).
 *
 * @param[in] dev       device to read from
 *
 * @return  measured temperature in centi Celsius
 * @return  INT16_MIN on error
 */
int16_t bmx280_read_temperature(bmx280_t* dev);

/**
 * @brief   Read air pressure value from the given BMX280 device
 *
 * The air pressure is returned in PA (Pascal).
 *
 * This function returns the pressure data that was measured when
 * bmx280_read_temperature() has been called last. So bmx280_read_temperature()
 * has to be called before.
 *
 * If bmx280_read_temperatue() did not succeed in acquiring a new set of sensor
 * data, the result of this function is undefined.
 *
 * @param[in]  dev      device to read from
 *
 * @return  air pressure in PA
 */
uint32_t bmx280_read_pressure(bmx280_t *dev);

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C) || defined(DOXYGEN)
/**
 * @brief   Read humidity value from the given BME280 device
 *
 * The humidity is returned in centi %RH (x.xx% relative humidity).
 *
 * This function returns the pressure data that was measured when
 * bmx280_read_temperature() has been called last. So bmx280_read_temperature()
 * has to be called before.
 *
 * If bmx280_read_temperatue() did not succeed in acquiring a new set of sensor
 * data, the result of this function is undefined.
 *
 * @param[in]  dev      device to read from
 *
 * @return  humidity in centi %RH (i.e. the percentage times 100)
 */
uint16_t bme280_read_humidity(bmx280_t *dev);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BMX280_H */
