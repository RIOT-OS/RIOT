/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SGP30_H
#define SGP30_H

/**
 * @defgroup    drivers_sgp30 SGP30 Gas Sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the Sensirion SGP30 Gas Sensor
 *
 * About
 * =====
 *
 * This driver provides an interface for the Sensirion SGP30 Gas Sensor.
 * The Datasheet can be found [here](https://www.mouser.com/pdfdocs/Sensirion_Gas_Sensors_SGP30_Datasheet_EN-1148053.pdf)
 *
 * After the sensor is powered up and starts measuring air quality for the
 * first 15s @ref sgp30_read_measurements calls will return fixed values of
 * of 400ppm CO2eq and 0ppb TVOC. Afterwards values should be read in regular
 * intervals of 1s for best operation of the dynamic baseline compensation
 * algorithm.
 *
 * The above is not done by default unless the `sgp30_strict` module is included.
 * In that case, if attempting to read before the is completed, then -EAGAIN
 * will be returned. After this periodic readings happen every 1s, and the last
 * value read is returned when calling @ref sgp30_read_measurements. A
 * timestamp is also added to @ref sgp30_data_t.
 *
 * The sensor features on-chip humidity compensation for the air quality
 * measurements. @ref sgp30_set_absolute_humidity can be used to change the
 * absolute humidity value used. See [SGP30 driver integration] (https://files.seeedstudio.com/wiki/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/res/Sensirion_Gas_Sensors_SGP30_Driver-Integration-Guide_HW_I2C.pdf)
 * for more on this. The baseline values for the correction algorithm can
 * also be tweaked with @ref sgp30_set_baseline. More on how to implement
 * dynamic baseline compensation can be seen at [SGP30 driver integration] (https://files.seeedstudio.com/wiki/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/res/Sensirion_Gas_Sensors_SGP30_Driver-Integration-Guide_HW_I2C.pdf).
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "periph/i2c.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     Set of measured values
 */
typedef struct {
    uint16_t tvoc;      /**< The last measurement of the IAQ-calculated Total
                             Volatile Organic Compounds in ppb. */
    uint16_t eco2;      /**< The last measurement of the IAQ-calculated
                             equivalent CO2 in ppm */
#ifdef MODULE_SGP30_STRICT
    uint32_t timestamp; /**< Timestamp of last measured value */
#endif
} sgp30_data_t;

/**
 * @brief     Set of measured raw values
 */
typedef struct {
    uint16_t raw_h2;        /**< raw H2 signal, only for testing purposes */
    uint16_t raw_ethanol;   /**< raw Ethanol signal, only for testing purposes */
} sgp30_raw_data_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C dev the sensor is connected to */
} sgp30_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    sgp30_params_t params;      /**< parameters of the sensor device */
#ifdef MODULE_SGP30_STRICT
    bool ready;                 /**< if initialization phase is over*/
    ztimer_t _timer;            /**< timer */
    sgp30_data_t _data;         /**< internal current data */
#endif
} sgp30_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int sgp30_init(sgp30_t *dev, const sgp30_params_t *params);

/**
 * @brief   Start air quality measurements, called on @ref sgp30_init
 *
 * @note    Must be called after every power-cycle or soft reset.
 *
 * @param[inout] dev        Device descriptor of the driver
 *
 * @retval  0         Success
 * @retval -PROTO     Sensor did not acknowledge command
 */
int sgp30_start_air_quality(sgp30_t *dev);

/**
 * @brief   Initialize the given device
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  version    The future set version
 *
 * @retval  0         Success
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int sgp30_read_future_set(sgp30_t *dev, uint16_t *version);

/**
 * @brief   Performs a soft reset on the device
 *
 * @warning Reset is performed via a "General Call". All devices on the
 *          same I2C bus that support the General Call mode will perform
 *          a reset.
 *
 * @param[in]   dev        Device descriptor of the driver
 *
 * @retval  0         Success
 * @retval -PROTO     Sensor did not acknowledge command
 */
int sgp30_reset(sgp30_t *dev);

#if defined(MODULE_SGP30_STRICT) || defined(DOXYGEN)
/**
 * @brief   If device is ready to start reading measurements
 *
 * @note    Only available if sgp30_strict is used
 *
 * @param[in]   dev        Device descriptor of the driver
 *
 * @retval true      If ready to read measurements
 * @retval false     If still in warm-up period of it @ref sgp30_start_air_quality
 *                   has not been called
 */
bool sgp30_ready(sgp30_t *dev);
#endif

/**
 * @brief   Read the serial number from the sensor
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  buf        Pre-allocated memory for returning the serial number
 * @param[in]   len        Length of the \p str buffer, must be 6
 *
 * @retval  0         Success
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int sgp30_read_serial_number(sgp30_t *dev, uint8_t *buf, size_t len);

/**
 * @brief   Read air quality signals
 *
 * @note For the first 15s after the “Init_air_quality” command (called
 * in the init function) the sensor is in an initialization phase during
 * which a “Measure_air_quality” command returns fixed values of 400ppm
 * CO2eq and 0ppb TVOC.
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  data       Air quality measurements
 *
 * @retval  0         Success
 * @retval -EAGAIN    Sensor is not yet ready
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int sgp30_read_measurements(sgp30_t *dev, sgp30_data_t *data);

/**
 * @brief   Set absolute humidity value for on-chop humidity compensation
 *
 * @note This function requires absolute humidity values, most sensors
 * output relative humidity, this can be calculated if temperature is also
 * known:
 *
 * AH = 216.7 * ((RH / 100.0) * 6.112 * exp(17.62 * T / (243.12 + T))) / (273.15 + T)
 *
 * Where RH is in 0-100%, and T is in C.
 *
 * (*) Approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[in]   humidity   Humidity in mg/m3 units. A values of 0 disables
 *                         humidity compensation, max value is 255999 mg/m3.
 *
 * @retval  0         Success
 * @retval -PROTO     Sensor did not acknowledge command
 */
int sgp30_set_absolute_humidity(sgp30_t *dev, uint32_t humidity);

/**
 * @brief   Set new baseline values
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[in]   data       Baseline values to be set
 *
 * @retval  0         Success
 * @retval -PROTO     Sensor did not acknowledge command
 */
int sgp30_set_baseline(sgp30_t *dev, sgp30_data_t *data);

/**
 * @brief   Returns baseline values
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  data       Current baseline values
 *
 * @retval  0         Success
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int sgp30_get_baseline(sgp30_t *dev, sgp30_data_t *data);

/**
 * @brief   Read raw signals H2 (sout_H2) and Ethanol(sout_EthOH)
 *
 * It returns the sensor raw sensor signals which are used as inputs for
 * the  on-chip calibration and baseline compensation algorithms.
 *
 * @param[in]   dev        Device descriptor of the driver
 * @param[out]  data       Raw measurement values
 *
 * @retval  0         Success
 * @retval -EBADMSG   CRC checksum didn't match
 * @retval -EPROTO    Sensor did not acknowledge command
 */
int sgp30_read_raw_measurements(sgp30_t *dev, sgp30_raw_data_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SGP30_H */
