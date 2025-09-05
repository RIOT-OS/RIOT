/*
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_sds011 SDS011 Laser Dust Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver SDS011 Laser Dust Sensor
 * @{
 *
 * @file
 * @brief       Interface for controlling SDS011 Laser Dust Sensor
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#include <stdbool.h>

#include "periph/gpio.h"
#include "periph/uart.h"
#include "mutex.h"
#include "sds011_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SDS011 wildcard address to address all devices
 */
#define SDS011_DEVID_WILDCARD            (0xFFFF)

/**
 * @brief   Named return values
 */
enum {
    SDS011_OK,               /**< all good */
    SDS011_INVALID_RESPONSE, /**< invalid response */
    SDS011_INVALID_CHKSUM,   /**< invalid checksum */
    SDS011_ERROR,            /**< internal error */
};

/**
 * @brief   Report mode of the SDS011 sensor
 */
typedef enum sds011_reporting_mode {
    SDS011_RMODE_ACTIVE = 0, /**< continuously reporting values */
    SDS011_RMODE_QUERY  = 1, /**< sensor needs to be queried */
} sds011_reporting_mode_t;

/**
 * @brief   Work/sleep mode of the SDS011 sensor
 */
typedef enum sds011_working_mode {
    SDS011_WMODE_SLEEP = 0, /**< laser & fan are disabled */
    SDS011_WMODE_WORK  = 1, /**< laser & fan are enabled */
} sds011_working_mode_t;

/**
 * @brief   Configuration parameters for SDS011 Laser Dust Sensor
 */
typedef struct {
    uart_t   uart;    /**< UART device the sensor is connected to */
    gpio_t   pwr_pin; /**< GPIO pin for disabling supply voltage for the sensor */
    uint16_t dev_id;  /**< Unique sensor device ID */
    bool     pwr_ah;  /**< Logic level of the power pin (true for active high) */
} sds011_params_t;

/**
 * @brief   Data type for storing SDS011 sensor readings
 */
typedef struct {
    uint16_t pm_2_5;  /**< Particulate Matter 2.5 concentration [0.1µg/m^3] */
    uint16_t pm_10;   /**< Particulate Matter 10 concentration [0.1µg/m^3] */
} sds011_data_t;

/**
 * @brief   callback for measurements actively reported by the SDS011 sensor
 */
typedef void (*sds011_callback_t)(sds011_data_t *data, void *ctx);

/**
 * @brief   Device descriptor definition for SDS011 Laser Dust Sensor
 */
typedef struct {
    sds011_params_t   params;        /**< parameters for SDS011 device */
    mutex_t           dev_lock;      /**< mutex to synchronize device access */
    mutex_t           cb_lock;       /**< mutex to synchronize callbacks */
    sds011_callback_t cb;            /**< callback deliver values async */
    void              *cbctx;        /**< user context for the callback */
    uint16_t          checksum;      /**< iteratively calculated checksum */
    uint8_t           rx_mem[SDS011_FRAME_RECV_LEN]; /**< receive buffer */
    uint8_t           pos;           /**< receive buffer position counter */
} sds011_t;

/**
 * @brief   Initialize SDS011 Laser Dust Sensor
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   device configuration
 *
 * @pre     @p dev != NULL
 * @pre     @p params != NULL
 *
 * @return              SDS011_OK on success
 * @return              SDS011_ERROR on error
 */
int sds011_init(sds011_t *dev, const sds011_params_t *params);

/**
 * @brief   Enable power supply of SDS011 laser dust sensor
 *
 * @param[in] dev       device descriptor
 *
 * @pre     @p dev != NULL
 */
void sds011_power_on(const sds011_t *dev);

/**
 * @brief   Disable power supply of SDS011 laser dust sensor
 *
 * @param[in] dev       device descriptor
 *
 * @pre     @p dev != NULL
 */
void sds011_power_off(const sds011_t *dev);

/**
 * @brief   Read measurement values from SDS011 laser dust sensor
 *
 * @param[in]  dev       device descriptor
 * @param[out] data      pointer for storing the values
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 * @pre     @p data != NULL
 */
int sds011_read(sds011_t *dev, sds011_data_t *data);

/**
 * @brief   Register measurement callback
 *
 *          The registered callback is executed when new measurements were
 *          received by the sensor. This function should be used together with
 *          active reporting mode of the sensor that automatically sends new
 *          measurements periodically (factory default setting of the sensor).
 *
 * @param[in]  dev       device descriptor
 * @param[in]  cb        function to be called for new values (NULL for disable)
 * @param[in]  ctx       context pointer that will be handed to the callback
 *
 * @return  SDS011_OK on success
 * @return  SDS011_ERROR when error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_register_callback(sds011_t *dev, sds011_callback_t cb, void *ctx);

/**
 * @brief   Get the current reporting mode of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[out]  mode      SDS011_RMODE_ACTIVE: continuously report new values
 *                        SDS011_RMODE_QUERY: new values need to be requested
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_get_reporting_mode(sds011_t *dev, sds011_reporting_mode_t *mode);

/**
 * @brief   Set the reporting mode of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[in]   mode      SDS011_RMODE_ACTIVE: continuously report new values
 *                        SDS011_RMODE_QUERY: new values need to be requested
 *
 * @note    This setting is persistent even after a full power-cycle!
 *          Factory default is SDS011_RMODE_ACTIVE
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_set_reporting_mode(sds011_t *dev, sds011_reporting_mode_t mode);

/**
 * @brief   Get current working mode of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[out]  mode      SDS011_WMODE_SLEEP: sensor is in sleep mode (~3 mA)
 *                        SDS011_WMODE_WORK: sensor is in working mode (~65 mA)
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_get_working_mode(sds011_t *dev, sds011_working_mode_t *mode);

/**
 * @brief   Set working mode of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[in]   mode      SDS011_WMODE_SLEEP: put to sleep mode (~3 mA)
 *                        SDS011_WMODE_WORK: put to working mode (~65 mA)
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_set_working_mode(sds011_t *dev, sds011_working_mode_t mode);

/**
 * @brief   Get current working period of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[out]  minutes   working period of the sensor in minutes
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_get_working_period(sds011_t *dev, uint8_t *minutes);

/**
 * @brief   Set working period of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[in]   minutes   0 - 30 new working period of the sensor in minutes
 *                        0 for continuous reporting mode
 *                        1 - 30 for a period of @p minutes
 *
 * @note    For values greater than 0, the active duration (fan, laser enabled)
 *          is always fixed to 30 seconds, while the sleep duration is adjusted
 *          to give an overall period of @p minutes.
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_set_working_period(sds011_t *dev, uint8_t minutes);

/**
 * @brief   Get firmware version of the sensor
 *
 * @param[in]   dev       device descriptor
 * @param[out]  year      year of the firmware version
 * @param[out]  mon       month of the firmware version
 * @param[out]  day       day of the firmware version
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 * @pre     @p year != NULL
 * @pre     @p mon != NULL
 * @pre     @p day != NULL
 */
int sds011_get_fw_version(sds011_t *dev, uint8_t *year, uint8_t *mon, uint8_t *day);

/**
 * @brief   Set device ID of the sensor
 *
 * @param[in]   dev            device descriptor
 * @param[in]   sens_dev_id   ID as one number (ID byte 1 MSB, ID byte 2 LSB)
 *
 * @note    This setting is persistent even after a full power-cycle!
 *          Factory default is an individual ID which is printed next to the
 *          serial number barcode. For the number xxxx-abab the ID is 0xabab.
 *
 * @return  SDS011_OK on success
 * @return  SDS011_INVALID_RESPONSE when response doesn't match the request
 * @return  SDS011_INVALID_CHKSUM when response checksum is invalid
 * @return  SDS011_ERROR when other error occurred
 *
 * @pre     @p dev != NULL
 */
int sds011_set_dev_id(sds011_t *dev, uint16_t sens_dev_id);

#ifdef __cplusplus
}
#endif

/** @} */
