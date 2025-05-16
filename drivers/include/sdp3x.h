/*
 * Copyright (C) 2019 Dirk Ehmen
 *               2020 Nishchay Agrawal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_sdp3x SDP3x temperature and differential pressure sensor
 * @ingroup     drivers_sensors
 * @{
 * @file
 * @brief       Device driver interface for the SDP3x sensor.
 *
 * @author      Dirk Ehmen       <ehmen@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @}
 */

#include "saul.h"
#include "xtimer.h"
#include "mutex.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief   Flags for sensor reading
 * @{
 */
#define SDP3X_FLAG_MASS_FLOW  0x02      /**< Use Mass Flow for Temperature Compensation */
#define SDP3X_FLAG_DIFF_PRESS 0x00      /**< Use Differential Pressure for Temperature Compensation */
#define SDP3X_FLAG_AVERAGE    0x01      /**< Average values */
#define SDP3X_FLAG_NO_AVERAGE 0x00      /**< Don't Average values */
/** @} */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C device which is used */
    uint8_t i2c_addr;               /**< I2C address */
    gpio_t irq_pin;                 /**< IRQ pin to check data ready */
} sdp3x_params_t;

/**
 * @brief   Device descriptor for the SDP3x sensor
 */
typedef struct {
    bool continuous_measurement;    /**< Device measuring data continuously */
    sdp3x_params_t params;          /**< Device initialization parameters */
    mutex_t mutex;                  /**< Mutex for this sdp3x dev */
} sdp3x_t;

/**
 * @brief   Measurement from SDP3x sensor
 */
typedef struct {
    int32_t differential_pressure;  /**< Differential Pressure measured
                                         in centiPa */
    int32_t temperature;            /**< Temperature measured in centi°C */
} sdp3x_measurement_t;

/**
 * @brief   Status and error return codes
 */
enum {
    SDP3x_OK = 0,             /**< all went as expected */
    SDP3x_CRCERROR = -1,      /**< CRC-Check failed */
    SDP3x_NODATA = -2,        /**< No Data available */
    SDP3x_IOERROR = -3,       /**< I/O error */
    SDP3x_WRONGSIZE = -4      /**< Wrong size of array for method */
};

/**
 *  @brief Initialize SDP3x
 *
 *  @param dev         sdp3x device
 *  @param params      sdp3x device params
 *
 *  @return            1 if device started
 */
int sdp3x_init(sdp3x_t *dev, const sdp3x_params_t *params);

/**
 *  @brief Start Continuous Measuring
 *
 *  @param dev          sdp3x device
 *  @param flags        stores Temperature compensation and averaging flags
 *                          0th bit-> 0 = No averaging, 1 = Average values
 *                          1st bit-> 0 = Differential pressure, 1 = Mass Flow
 *
 *  @return             1 if device started
 */
int8_t sdp3x_start_continuous(sdp3x_t *dev, uint8_t flags);

/**
 *  @brief read temperature and differential pressure
 *
 *  @param result       struct to store result
 *  @param dev          sdp3x device
 *
 *  @return             1 on success
 */

int8_t sdp3x_read_continuous(sdp3x_measurement_t *result, sdp3x_t *dev);

/**
 *  @brief  Read current temperature value
 *
 *  @param dev           sdp3x device
 *  @param flags         stores Temperature compensation
 *                           1st bit-> 0 = Differential pressure, 1 = Mass Flow
 *
 *  @return              Current temperature in celsius multiplied by 100
 *                       to preserve accuracy and return in integer
 */
int32_t sdp3x_read_single_temperature(sdp3x_t *dev, uint8_t flags);

/**
 *  @brief  Read current differential_pressure value
 *
 *  @param dev           sdp3x device
 *  @param flags         stores Temperature compensation
 *                           1st bit-> 0 = Differential pressure, 1 = Mass Flow
 *
 *  @return              current differential pressure in centipascal
 */
int32_t sdp3x_read_single_differential_pressure(sdp3x_t *dev,
                                                uint8_t flags);

/**
 *  @brief  read temperature and differential pressure
 *
 *  @param dev           sdp3x device
 *  @param flags         stores Temperature compensation
 *                           1st bit-> 0 = Differential pressure, 1 = Mass Flow
 *  @param result        Values are stored in this struct
 *
 *  @return              1 on success
 */
int8_t sdp3x_read_single_measurement(sdp3x_t *dev, uint8_t flags,
                                     sdp3x_measurement_t *result);

/**
 *  @brief  Stop Continuous Measuring
 *
 *  It takes 500 us till next command.
 *
 *  @param                   dev device
 *  @param continuous_timer  timer running to trigger reading data
 *
 *  @return                  0 if measurement stopped
 */
int8_t sdp3x_stop_continuous(sdp3x_t *dev, xtimer_t *continuous_timer);

/**
 *  @brief  Resets all I2C devices
 *
 *  @param                      dev device
 *
 *  @return                     0 if reset is started
 */
int8_t sdp3x_soft_reset(const sdp3x_t *dev);

/**
 *  @brief  Activates sleep mode
 *
 *  Only activates Sleep Mode if the device is in Idle Mode
 *
 *  @param                      dev device
 *
 *  @return                     0 if sleep is activated
 */
int8_t sdp3x_enter_sleep(const sdp3x_t *dev);

/**
 *  @brief  Exit sleep mode
 *
 *  @param                      dev device
 *
 *  @return                     0 if sleep is deactivated
 */
int8_t sdp3x_exit_sleep(const sdp3x_t *dev);

#ifdef __cplusplus
}
#endif
