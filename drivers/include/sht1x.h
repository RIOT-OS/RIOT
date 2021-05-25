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

#include "sht1x_types.h"
#include "sht1x_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_AUTO_INIT_ACTUATORS_SENSORS_SHT1X
/**
 * @brief   Array of all auto-initialized SHT1X device descriptors
 *
 * @note    Only available if module @ref sys_auto_init_actuators_sensors_sht1x
 *          is used
 */
extern sht1x_dev_t sht1x_devs[SHT1X_NUMOF];

/**
 * @brief   If `sht1x` is auto-initialized, device descriptors can be retrieved by number
 * @param   num     Number of the SHT1X device to retrieve the device descriptor of
 * @return  The device descriptor of the device with number @p num
 * @retval  `NULL`  @p num is out of range
 *
 * @note    Only available if module @ref sys_auto_init_actuators_sensors_sht1x
 *          is used
 */
static inline sht1x_dev_t * sht1x_get_dev(unsigned num)
{
    if (num >= SHT1X_NUMOF) {
        return NULL;
    }

    return &sht1x_devs[num];
}
#endif /* MODULE_AUTO_INIT_ACTUATORS_SENSORS_SHT1X */

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
 * @retval -EBADMSG   CRC-8 checksum didn't match (--> Retry)
 * @retval -EINVAL    Passed `NULL` for dev or for both `temp` and `hum`
 * @retval -EBADMSG   CRC checksum didn't match
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

#endif /* SHT1X_H */
/** @} */
