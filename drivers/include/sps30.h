/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_sps30 SPS30 Particulate Matter Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Sensirion SPS30 Particulate Matter Sensor
 *
 * About
 * =====
 *
 * This driver provides an interface for the Sensirion SPS30 Sensor.
 * The Datasheet can be found [here](https://1n.pm/oqluM).
 * For now only I2C mode is supported.
 * I2C speed must be set to standard mode (100 kbit/s)
 *
 * Wiring
 * ======
 *
 * In ASCII-land the connector side of the sensor would look like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  +------------------------------------------------------------------------+
 *  |                                             ____________________       |
 *  |                                          __|                    |__    |
 *  |                                         |__  (1) (2) (3) (4) (5) __|   |
 *  |                                            |                    |      |
 *  |                                            |____________________|      |
 *  |                                                                        |
 *  +------------[#]------------[#]------------[#]------------[#]------------+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The numbers refer to following pin names:
 *
 * | Pin Nr. | SPS30 Signal Name | Connect to | Notes                         |
 * |---------|-------------------|------------|-------------------------------|
 * | Pin 1   | VDD               | 5 V        | should be within +-10 %       |
 * | Pin 2   | I2C_SDA / UART_RX | SDA*       | config by SPS30_PARAM_I2C_DEV |
 * | Pin 3   | I2C_SCL / UART_TR | SCL*       | config by SPS30_PARAM_I2C_DEV |
 * | Pin 4   | SEL               | GND        |                               |
 * | Pin 5   | GND               | GND        |                               |
 *
 *   *The SCL and SDA pins of the SPS30 sensor are open drain so they must be
 *    pulled high. Consider that internal pull resistors might be too weak.
 *    So using external 10 kOhm resistors is recommended for that.
 *
 * @{
 *
 * @file
 * @brief       Driver for the Sensirion SPS30 Particulate Matter Sensor
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef SPS30_H
#define SPS30_H

#include <stdbool.h>
#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     SPS30 device parameters
 */
typedef struct {
    i2c_t i2c_dev;                /**< I2C dev the sensor is connected to */
} sps30_params_t;

/**
 * @brief     SPS30 device instance
 */
typedef struct {
    sps30_params_t p;             /**< parameters of the sensor device */
} sps30_t;

/**
 * @brief     Set of measured particulate matter values
 *
 * @warning   Do not change the member order, as it will break the code that
 *            populates the values in #sps30_read_measurement()
 *
 */
typedef struct {
    float mc_pm1;       /**< Mass concentration of all particles <= 1µm [µg/m^3] */
    float mc_pm2_5;     /**< Mass concentration of all particles <= 2.5µm [µg/m^3] */
    float mc_pm4;       /**< Mass concentration of all particles <= 4µm [µg/m^3] */
    float mc_pm10;      /**< Mass concentration of all particles <= 10µm [µg/m^3] */
    float nc_pm0_5;     /**< Number concentration of all particles <= 0.5µm [#/cm^3] */
    float nc_pm1;       /**< Number concentration of all particles <= 1µm [#/cm^3] */
    float nc_pm2_5;     /**< Number concentration of all particles <= 2.5µm [#/cm^3] */
    float nc_pm4;       /**< Number concentration of all particles <= 4µm [#/cm^3] */
    float nc_pm10;      /**< Number concentration of all particles <= 10µm [#/cm^3] */
    float ps;           /**< Typical particle size [µm] */
} sps30_data_t;

/**
 * @brief     SPS30 error codes (returned as negative values)
 */
typedef enum {
    SPS30_OK = 0,     /**< Everything went fine */
    SPS30_CRC_ERROR,  /**< The CRC check of received data failed */
    SPS30_I2C_ERROR   /**< Some I2C operation failed */
} sps30_error_code_t;

/**
 * @brief     Seconds the fan cleaning process takes in seconds
 */
#define SPS30_FAN_CLEAN_S    (10U)

/**
 * @brief     Length of serial and article code string
 */
#define SPS30_SER_ART_LEN    (32U)

/**
 * @brief     Default fan auto-clean interval in seconds (1 week)
 */
#define SPS30_DEFAULT_ACI_S  (604800UL)

/**
 * @defgroup drivers_SPS30_config     SPS30 PM Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief     Maximum number of automatic retries on communication errors
 *
 * @details   If no delays happen between individual requests to the sensor, it
 *            may happen that the sensor is not yet ready to serve data.
 *            Handling this within the driver simplifies application code by
 *            omitting sleep handling or retries there.
 *            This value may be overwritten to 0 if more fine-grained feedback
 *            is required or even increased if the device is connected over
 *            suboptimal wiring.
 *
 */
#ifndef CONFIG_SPS30_ERROR_RETRY
#define CONFIG_SPS30_ERROR_RETRY    (500U)
#endif
/** @} */

/**
 * @brief       Initialize SPS30 sensor driver.
 *              On success the measurement mode will be active after calling.
 *
 * @param[out]  dev         Pointer to an SPS30 device handle
 * @param[in]   params      Parameters for device initialization
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_init(sps30_t *dev, const sps30_params_t *params);

/**
 * @brief       Enable the measurement action.
 *
 * @details     Starting the measurement activates the fan of the sensor and
 *              increases the power consumption from below 8 mA to an average
 *              of 60 mA. When the fan is starting up the consumption can reach
 *              up to 80 mA for around 200 ms.
 *              The measurement mode will stay active until either the power is
 *              turned off, a stop is requested (@ref sps30_stop_measurement),
 *              or the device is reset (#sps30_reset).
 *
 * @param[in]   dev               Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_start_measurement(const sps30_t *dev);

/**
 * @brief       Stops the measurement action.
 *
 * @details     Stopping the measurement sets the device back to idle mode.
 *
 * @param[in]   dev               Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_stop_measurement(const sps30_t *dev);

/**
 * @brief       Ask the device if a measurement is ready for reading.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  error      Pre-allocated memory to return #sps30_error_code_t
 *                         or NULL if not interested
 *
 * @return      true       if a new measurement is available
 * @return      false      if no new measurement is available
 */
bool sps30_data_ready(const sps30_t *dev, int *error);

/**
 * @brief       Read a set of particulate matter measurements.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  data       Pre-allocated memory to return measurements
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_read_measurement(const sps30_t *dev, sps30_data_t *data);

/**
 * @brief       Read the fan auto-clean interval.
 *
 * @details     The default value is 604800 seconds (1 week).
 *              See also @ref sps30_start_fan_clean.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  seconds    Pre-allocated memory for returning the interval,
 *                         0 stands for disabled auto-clean
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_read_ac_interval(const sps30_t *dev, uint32_t *seconds);

/**
 * @brief       Write the fan auto-clean interval.
 *
 * @details     The new value will be effective immediately after writing but
 *              reading the updated value is only possible after resetting the
 *              sensor.
 *              This setting is persistent across resets and powerdowns. But if
 *              the sensor is powered off, the active time counter starts from
 *              zero again. If this is expected to happen, a fan cleaning cycle
 *              should be triggered manually at least once a week.
 *              See also @ref sps30_start_fan_clean.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  seconds    The new interval in seconds, 0 for disable
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_write_ac_interval(const sps30_t *dev, uint32_t seconds);

/**
 * @brief       Run a fan cleaning cycle manually.
 *
 * @details     This will spin up the fan to maximum speed to blow out dust for
 *              for 10 seconds. No new measurement values are available until
 *              the cleaning process is finished.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_start_fan_clean(const sps30_t *dev);

/**
 * @brief       Read the article code from the sensor as string.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  str        Pre-allocated memory for returning the article code
 * @param[in]   len        Length of the \p str buffer, must be 32
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_read_article_code(const sps30_t *dev, char *str, size_t len);

/**
 * @brief       Read the serial number from the sensor as string.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 * @param[out]  str        Pre-allocated memory for returning the serial number
 * @param[in]   len        Length of the \p str buffer, must be 32
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_read_serial_number(const sps30_t *dev, char *str, size_t len);

/**
 * @brief       Reset the sensor.
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_reset(const sps30_t *dev);

/**
 * @brief       Put the sensor in sleep mode
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_sleep(const sps30_t *dev);

/**
 * @brief       Wake up sensor from sleep mode (returns sensor to Idle mode)
 *
 * @param[in]   dev        Pointer to an SPS30 device handle
 *
 * @return      #SPS30_OK on success, negative #sps30_error_code_t on error
 */
int sps30_wakeup(const sps30_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* SPS30_H */
/** @} */
