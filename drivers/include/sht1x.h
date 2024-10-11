/*
 * Copyright 2009 Freie Universitaet Berlin (FUB)
 *           2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sht1x SHT10/SHT11/SHT15 Humidity and Temperature Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for Sensirion SHT10/SHT11/SHT15 Humidity and Temperature
                Sensor
 * @{
 *
 * @file
 * @brief       SHT10/SHT11/SHT15 Device Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SHT1X_H
#define SHT1X_H

#include <stdint.h>
#include <periph/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Possible configuration (=status byte) values of the SHT10/11/15
 *
 * These values can be or'ed together to get the configuration.
 */
typedef enum {
    /** Use 8/12 bit resolution instead of 12/14 bit for temp/hum */
    SHT1X_CONF_LOW_RESOLUTION   = 0x01,
    /** Don't upload calibration data to register to safe 10 millisec */
    SHT1X_CONF_SKIP_CALIBRATION = 0x02,
    /** Waste 8mA at 5V to increase the sensor temperature up to 10°C */
    SHT1X_CONF_ENABLE_HEATER    = 0x04,
    /** Skip the CRC check (and reading the CRC byte) to safe time */
    SHT1X_CONF_SKIP_CRC         = 0x08,
} sht1x_conf_t;

/**
 * @brief Possible values for Vdd (measured temperature depends on it)
 */
typedef enum {
    SHT1X_VDD_5_0V = 0,
    SHT1X_VDD_4_0V = 1,
    SHT1X_VDD_3_5V = 2,
    SHT1X_VDD_3_0V = 3,
    SHT1X_VDD_2_5V = 4,
} sht1x_vdd_t;

/**
 * @brief SHT10/11/15 temperature humidity sensor
 */
typedef struct {
    gpio_t  clk;      /**< GPIO connected to the clock pin of the SHT1X */
    gpio_t  data;     /**< GPIO connected to the data pin of the SHT1X */
    int16_t temp_off; /**< Offset to add to the measured temperature */
    int16_t hum_off;  /**< Offset to add to the measured humidity */
    uint8_t conf;     /**< Status byte (containing configuration) of the SHT1X */
    uint8_t vdd;      /**< Supply voltage of the SHT1X (as sht1x_vdd_t) */
} sht1x_dev_t;

/**
 * @brief Parameters required to set up the SHT10/11/15 device driver
 */
typedef struct {
    gpio_t      clk;  /**< GPIO connected to the clock pin of the SHT1X */
    gpio_t      data; /**< GPIO connected to the data pin of the SHT1X */
    sht1x_vdd_t vdd;  /**< The supply voltage of the SHT1X */
} sht1x_params_t;

/**
 * @brief             Initialize the SHT10/11/15 sensor
 *
 * @param  dev        SHT1X sensor to initialize
 * @param  params     Information on how the SHT1X is connected to the board
 *
 * @retval  0         Success
 * @retval -EIO       IO failure (`gpio_init()` failed)
 * @retval -EPROTO    Sensor did not acknowledge reset command
 */
int sht1x_init(sht1x_dev_t *dev, const sht1x_params_t *params);

/**
 * @brief             Calculate the temperature from the raw input
 * @note              This internal function is exposed for unit tests
 *
 * @param dev         Device from which the raw value was received
 * @param raw         The raw (unprocessed) temperature value
 *
 * @return            The correct temperature in E-02 °C
 * @retval INT16_MIN  Passed `NULL` for parameter `dev` or `dev->vdd`
 */
int16_t sht1x_temperature(const sht1x_dev_t *dev, uint16_t raw);

/**
 * @brief             Calculate the relative humidity from the raw input
 * @note              This internal function is exposed for unit tests
 *
 * @param dev         Device from which the raw value was received
 * @param raw         The raw (unprocessed) temperature value
 * @param temp        The temperature at which the humidity was measure in
 *                    E-02 °C
 *
 * @return            The correct temperature in E-02 %
 * @retval -1         Passed `NULL` for parameter `dev`
 */
int16_t sht1x_humidity(const sht1x_dev_t *dev, uint16_t raw, int16_t temp);

/**
 * @brief             Read the current temperature
 *
 * @param dev         SHT1X sensor to read
 * @param temp        Store the measured temperature in E-02 °C here
 * @param hum         Store the measured relative humidity in E-02 % here
 *
 * @retval  0         Success
 * @retval -EIO       IO failure (`gpio_init()` failed)
 * @retval -EBADMSG   CRC-8 checksum didn't match
 * @retval -EINVAL    Passed `NULL` for dev or for both `temp` and `hum`
 * @retval -ECANCELED Measurement timed out
 * @retval -EPROTO    Sensor did not acknowledge command
 *
 * For either `temp` or `hum` `NULL` can be passed, if only one value is of
 * interest. Passing `NULL` for `hum` speeds up the communication, but
 * passing `NULL` for `temp` does not. The temperature value is required to
 * calculate the relative humidity from the raw input. So the temperature is
 * measured in any case, it is just not returned if `temp` is `NULL`.
 */
int sht1x_read(const sht1x_dev_t *dev, int16_t *temp, int16_t *hum);

/**
 * @brief             Apply the given configuration (= status byte) to
 *
 * @param dev         SHT1X device to configure
 * @param conf        Configuration to apply
 *
 * @retval  0         Configuration applied
 * @retval -EINVAL    Called with `dev == NULL`
 * @retval -EIO       I/O error (`gpio_init()` failed)
 * @retval -EPROTO    Sensor did not acknowledge command
 * @retval -ECANCELED Sensor did not apply configuration
 * @retval -EBADMSG   CRC checksum error while verifying uploaded configuration
 */
int sht1x_configure(sht1x_dev_t *dev, sht1x_conf_t conf);

/**
 * @brief             Read the status byte of an SHT1X sensor
 *
 * @param dev         SHT1X device to receive the status from
 * @param status      Store the received status byte here
 *
 * @retval  0         Configuration applied
 * @retval -EINVAL    Called with `dev == NULL`
 * @retval -EIO       I/O error (`gpio_init()` failed)
 * @retval -EPROTO    Sensor did not acknowledge command
 * @retval -EBADMSG   CRC checksum didn't match
 */
int sht1x_read_status(sht1x_dev_t *dev, uint8_t *status);

/**
 * @brief             Reset the sensor's configuration to default values
 *
 * @param dev         SHT1X device to reset
 *
 * @retval  0         Reset successful
 * @retval -EINVAL    Called with `dev == NULL`
 * @retval -EIO       I/O error (`gpio_init()` failed)
 * @retval -EPROTO    Sensor did not acknowledge reset command
 */
int sht1x_reset(sht1x_dev_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SHT1X_H */
