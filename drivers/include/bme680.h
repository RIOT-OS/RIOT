/*
 * Copyright (C) 2019 Mesotic SAS
 *               2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bme680 BME680 Temperature/Humidity/Pressure/Gas sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for the Bosch BME680 sensor
 *
 * ### Introduction
 *
 * The driver allows the use of BME680 sensors connected either via I2C or SPI.
 * Instead of implementing a complete driver, it simply uses the
 * [BME680 vendor driver](https://github.com/BoschSensortec/BME680_driver)
 * written and maintained by Bosch Sensortec as a package.
 *
 * Even though this driver interface provides an easy-to-use API, the vendor's
 * driver API can still be used directly. This becomes necessary, for example,
 * if the settings of the ambient temperature have to be updated from
 * measurements with other sensors for gas measurement.
 *
 * All functions of the vendor's driver API require a reference to a
 * sensor device structure of type `struct bme680_dev`. Use macro
 * @ref BME680_SENSOR(dev) for a given device descriptor of type
 * @ref bme680_t to the according sensor device structure of type
 * `struct bme680_dev`, for example:
 *
 * ```c
 * bme680_t dev;
 * ...
 * BME680_SENSOR(dev).amb_temp = value_from_other_sensor;
 * bme680_set_sensor_settings(BME680_GAS_MEAS_SEL, &BME680_SENSOR(dev));
 * ```
 *
 * Refer to the code documentation at
 * [GitHub](https://github.com/BoschSensortec/BME680_driver)
 * for detailed information on the API of the vendor driver.
 *
 * ### Sensor Operation Modes
 *
 * The BME680 sensor supports only two modes, sleep mode and forced mode, in
 * which measurements are taken. After the power-on sequence, the sensor
 * automatically starts in sleep mode. To start a measurement, the sensor
 * must switch to forced mode. In this mode it performs exactly one
 * measurement of temperature, pressure, humidity and gas in this order, the
 * so-called TPHG measurement cycle. After executing this TPHG measurement
 * cycle, the raw data from the sensor is available and the sensor
 * automatically returns to sleep mode
 *
 * ### Ambient Temperature
 *
 * The sensor is initialized with a fixed ambient temperature defined by the
 * parameter settings in @ref bme680_params. However, precise gas measurements
 * require the calculation of the heating resistance based on the ambient
 * temperature.
 *
 * The temperature of the internal temperature sensor is typically higher
 * than the actual ambient temperature due to the self-heating of the sensor.
 * element. It should therefore not be used to set the ambient temperature
 * unless gas measurements are very infrequent and self-heating is negligible.
 * Rather another temperature sensor should be used for that purpose.
 *
 * Function @ref bme680_set_ambient_temp can be used to change the ambient
 * temperature.
 *
 * ### Using the Sensor
 *
 * Using the BME680 consists of the following steps
 *
 * 1. Trigger the sensor with @ref bme680_force_measurement to change to the
 *    forced mode and perform a THPG cycle.
 * 2. Wait at least the time returned by @ref bme680_get_duration until the
 *    THPG cycle is finished.
 * 3. Use @ref bme680_get_data to fetch raw sensor data and convert them to the
 *    resulting sensor values
 *
 * ### Driver Configuration
 *
 * BME680 sensors are connected either via I2C or SPI. Which interface is used
 * by which BME680 sensor is defined by the parameters in @ref bme680_params.
 * The respective driver implementation is enabled by the modules `bme680_i2c`
 * and `bme680_spi`. Several BME680 sensors and a mixed configuration of I2C
 * and SPI can be used in one application.
 * ```
 * USEMODULE='bme680_spi bme680_i2c' make BOARD=... -C tests/drivers/bme680
 * ```
 *
 * The vendor driver allows the use of floating point conversions. In order
 * to use these floating point conversions, module `bme680_fp` has to
 * be enabled:
 * ```
 * USEMODULE='bme680_fp bme680_i2c' make BOARD=... -C tests/drivers/bme680
 * ```
 *
 * @{
 * @file
 * @brief       Interface definition for the Bosch BME680 sensor
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BME680_H
#define BME680_H

#include "periph/i2c.h"
#include "periph/spi.h"

#ifdef MODULE_BME680_FP
#define BME680_FLOAT_POINT_COMPENSATION
#endif

#include "bme680_hal.h"
#include "bme680_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    I2C address when SDO pin is LOW
 */
#define BME680_I2C_ADDR_1   (0x76)

/**
 * @brief    I2C address when SDO pin is HIGH
 */
#define BME680_I2C_ADDR_2   (0x77)

/**
 * @brief   Converts a BME680 device descriptor to the BME680 sensor device
 *          structure for the vendor BME680 device driver.
 */
#define BME680_SENSOR(d)    (*((struct bme680_dev *)d))

/**
 * @brief   Named return values
 */
enum {
    BME680_NULL_PTR     = -1,   /**< NULL pointer check failed. */
    BME680_COM_FAILED   = -2,   /**< Communication with the device failed. */
    BME680_NO_DEV       = -3,   /**< Device doesn't exist. */
    BME680_INVALID      = -4,   /**< Invalid value or length. */
    BME680_NO_NEW_DATA  = -5,   /**< No new data. */
};

#ifdef DOXYGEN
/**
 * @brief   BME680 sensor field data
 */
typedef struct bme680_field_data {
    uint8_t status;     /**< Status for new data, gas measurement valid and
                             heater stable. Use `BME680_NEW_DATA_MSK`,
                             `BME680_GASM_VALID_MSK` and BME680_HEAT_STAB_MSK
                             to check for the status. */
    uint8_t gas_index;  /**< Index of used heater profile */
    uint8_t meas_index; /**< Measurement index */
#ifndef MODULE_BME680_FP
    int16_t temperature;        /**< Temperature in degree Celsius x 100 */
    uint32_t pressure;          /**< Pressure in Pascal */
    uint32_t humidity;          /**< Relative humidity in percent x 1000 */
    uint32_t gas_resistance;    /**< Gas resistance in ohms */
#else /* MODULE_BME680_FP */
    float temperature;          /**< Temperature in degree Celsius */
    float pressure;             /**< Pressure in Pascal */
    float humidity;             /**< Relative humidity in percent */
    float gas_resistance;       /**< Gas resistance in ohms */
#endif /* MODULE_BME680_FP */
};

#endif /* DOXYGEN */

/**
 * @brief   Shortcut type definition for BME680 sensor field data
 */
typedef struct bme680_field_data bme680_field_data_t;

/**
 * @brief   Shortcut type definition for BME680 sensor device structure
 * @see [struct bme680_dev](https://github.com/BoschSensortec/BME680_driver/blob/9014031fa00a5cc1eea1498c4cd1f94ec4b8ab11/bme680_defs.h#L496-L530)
 */
typedef struct bme680_dev bme680_dev_t;

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
 * @brief   BME680 device initialization parameters
 */
typedef struct {
    uint8_t ifsel;              /**< Interface selection */
    uint8_t temp_os;            /**< Temperature oversampling */
    uint8_t hum_os;             /**< Humidity oversampling */
    uint8_t pres_os;            /**< Pressure oversampling */
    uint8_t filter;             /**< IIR filter coefficient */
    uint8_t gas_measure;        /**< Enable gas measurement */
    uint16_t heater_dur;        /**< Heater duration in ms */
    uint16_t heater_temp;       /**< Heater temperature in °C */
    uint8_t power_mode;         /**< Power mode (sleep or forced) */
    uint8_t settings;           /**< Settings used */
    bme680_intf_t intf;         /**< Hardware interface parameters */
} bme680_params_t;

/**
 * @brief   BME680 device descriptor
 */
typedef struct {
    struct bme680_dev sensor; /**< Inherited device structure from vendor API */
    bme680_intf_t intf;       /**< Device interface */
} bme680_t;

/**
 * @brief   References to BME680 sensor devices used by the HAL functions
 */
extern bme680_t *bme680_devs[];

/**
 * @brief   Number of initialized BME680 sensor devices in bme680_devs
 */
extern unsigned int bme680_devs_numof;

/**
 * @brief   Initialize the BME680 sensor.
 *
 * @param[in,out]   dev     device descriptor of the sensor to initialize
 * @param[in]       params  configuration parameters
 *
 * @return 0 on success
 * @return < 0 on error
  */
int bme680_init(bme680_t *dev, const bme680_params_t *params);

/**
 * @brief    Force a single TPHG measurement cycle
 *
 * The function triggers the sensor to start one THPG measurement cycle. The
 * duration of the TPHG measurement cycle depends on the selected parameters.
 * It can vary from 1.25 ms to 4.5 seconds. The duration of the measurement
 * cycle can be determined with the #bme680_get_duration function.
 *
 * @param[in,out]   dev     device descriptor of the sensor
 *
 * @return 0 on success
 * @return < 0 on error
 */
int bme680_force_measurement(bme680_t *dev);

/**
 * @brief    Duration one THPG measurement cycle
 *
 * This function determines the duration of one THPG measurement cycle
 * according to the selected parameter settings. The duration can be used
 * to wait for the measurement results once a THPG measurement has been
 * started with #bme680_force_measurement.
 *
 * @param[in,out]   dev     device descriptor of the sensor
 *
 * @return  duration of one THPG measurement cycle in milliseconds.
 * @return  < 0 on error
 */
int bme680_get_duration(bme680_t* dev);

/**
 * @brief   Get results of a TPHG measurement
 *
 * The function returns the results of a TPHG measurement that has been
 * started before with #bme680_force_measurement. For that purpose, the
 * function fetches the raw sensor data and converts them into sensor values.
 * If the measurement is still running, the function fails and returns
 * invalid values.
 *
 * @param[in,out]   dev     device descriptor of the sensor
 * @param[out]      data    pointer to a data structure with the field data
 *
 * @return 0 on success
 * @return < 0 on error
 */
int bme680_get_data(bme680_t* dev, bme680_field_data_t *data);

/**
 * @brief   Set the ambient temperature
 *
 * The function sets the ambient temperature for the calculation of the heater
 * resistance.
 *
 * @param[in,out]   dev     device descriptor of the sensor
 * @param[in]       temp    ambient temperature in degC.
 *
 * @return 0 on success
 * @return < 0 on error
 */
int bme680_set_ambient_temp(bme680_t* dev, int8_t temp);

#ifdef __cplusplus
}
#endif

#endif /* BME680_H */
/** @} */
