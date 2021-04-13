/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pms5003 sensors
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the Plantower PMS5003 Particulate Matter Sensor
 *
 * About
 * =====
 *
 * This driver provides an interface for the Plantower PMS5003 Sensor.
 * The Datasheet can be found [here](https://www.aqmd.gov/docs/default-source/aq-spec/resources-page/plantower-pms5003-manual_v2-3.pdf).
 *
 * For now only UART mode is supported, but an i2c version of the sensor
 * is also available.
 *
 * @note The sensor fan has a warmup period of 30s, so measurement values
 * under that time when waking up should be discarded.
 *
 * @{
 *
 * @file
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PMS5003_H
#define PMS5003_H

#include <stdbool.h>

#include "ztimer.h"
#include "mutex.h"
#include "byteorder.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_PMS5003_config     PMS5003 PM Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */

/**
 * @brief     PMS5003 deployment setting
 *
 * The PMS5003 sensor outputs two set of PM* values, one calibrated for indoor
 * environment and another one for atmospheric environment, set this value
 * according to your deployment.
 *
 */
#ifndef CONFIG_PMS5003_INDOOR_ENVIRONMENT
#define CONFIG_PMS5003_INDOOR_ENVIRONMENT       1
#endif
/** @} */

/**
 * @brief     Set of measured particulate matter values as sent by the device
 *
 */
typedef union __attribute__((packed)) {
    struct {
        uint16_t mc_pm_1;       /**< PM1.0 ug/m3 (ultrafine particles) <= 1µm [µg/m^3] */
        uint16_t mc_pm_2p5;     /**< PM2.5 ug/m3 (combustion particles, organic compounds, metals) <= 2.5µm [µg/m^3] */
        uint16_t mc_pm_10;      /**< PM10 ug/m3  (dust, pollen, mould spores) <= 10µm [µg/m^3] */
        uint16_t amc_pm_1;      /**< PM1.0 ug/m3 (atmospheric environment) <= 1µm [µg/m^3] */
        uint16_t amc_pm_2p5;    /**< PM2.5 ug/m3 (atmospheric environment) <= 2.5µm [µg/m^3] */
        uint16_t amc_pm_10;     /**< PM10 ug/m3  (atmospheric environment) <= 10µm [µg/m^3] */
        uint16_t nc_pm_0p3;     /**< Number concentration of all particles <= 0.3µm [#/cm^3] */
        uint16_t nc_pm_0p5;     /**< Number concentration of all particles <= 0.5µm [#/cm^3] */
        uint16_t nc_pm_1;       /**< Number concentration of all particles <= 1µm [#/cm^3] */
        uint16_t nc_pm_2p5;     /**< Number concentration of all particles <= 2.5µm [#/cm^3] */
        uint16_t nc_pm_5;       /**< Number concentration of all particles <= 5µm [#/cm^3] */
        uint16_t nc_pm_10;      /**< Number concentration of all particles <= 10µm [#/cm^3] */
        uint16_t reserved;      /**< Reserved */
    } values;                   /**< Measurement values struct */
    uint16_t buffer[13];        /**< Convenience uint16_t buffer */
} pms5003_data_t;

/**
 * @brief     Particulate matter sensor UART frame parser buffer
 *
 *  Data is sent in bigendian so needs to be converted to host order.
 */
typedef struct {
    uint8_t idx;                /**< Parsing index in bytes */
    union __attribute__((packed)) {
        be_uint16_t be[16];     /**< start + len + data (13) + crc */
        uint8_t bytes[32];      /**< convenience uint8_t buffer */
    } buf;                      /**< buffer union */
} pms5003_parser_t;

/**
 * @brief   Available read modes
 */
typedef enum {
    PMS5003_PASSIVE_MODE = 0,   /**< Passive reading mode (on demand) */
    PMS5003_ACTIVE_MODE,        /**< Active reading mode (continuous) */
} pms5003_mode_t;

/**
 * @brief   Definition of available sleep states
 */
typedef enum {
    PMS5003_SLEEP = 0,          /**< Device is asleep */
    PMS5003_RUNNING,            /**< Device is running */
} pms5003_sleep_t;

/**
 * @brief   PMS5003 device state
 * @internal
 */
typedef enum {
    PMS5003_STATE_IDLE = 0,                 /**< Currently parsing a frame */
    PMS5003_STATE_READ_REQUEST,             /**< Currently parsing a frame */
    PMS5003_STATE_MODE_ACTIVE_REQUEST,      /**< Parsed valid frame */
    PMS5003_STATE_MODE_PASSIVE_REQUEST,     /**< Parsed valid frame */
    PMS5003_STATE_SLEEP_REQUEST,            /**< Parsed invalid frame */
    PMS5003_STATE_SLEEP,                    /**< Parsed invalid frame */
    PMS5003_STATE_WAKEUP_REQUEST,           /**< Parsed invalid frame */
} pms5003_state_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uart_t uart;        /**< The UART device */
    gpio_t reset_pin;   /**< Reset pin, active low */
    gpio_t enable_pin;  /**< Enable pin, active high*/
} pms5003_params_t;

/**
 * @brief   pms5003 callback type
 */
typedef void (*pms5003_callback_t)(const pms5003_data_t *data, void *arg);

/**
 * @brief   Internal state management
 */
typedef struct {
    mutex_t busy;                      /**< Internal, busy lock */
    mutex_t timeout;                   /**< Internal, timeout mutex lock */
    ztimer_t busy_timer;               /**< Internal, busy timer */
    pms5003_parser_t parser;           /**< Internal, frame parser */
} pms5003_internal_t;

/**
 * @brief   Callbacks for an PMS5003 particulate matter sensor
 */
typedef struct pms5003_callbacks {
    struct pms5003_callbacks *next; /**< Next registered callbacks */
    pms5003_callback_t cb;          /**< Called when data was received */
    void *arg;                      /**< Data to pass to the callbacks */
} pms5003_callbacks_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    pms5003_params_t params;            /**< Parameters of the sensor device */
    pms5003_state_t state;              /**< Device state information */
    pms5003_mode_t mode;                /**< Operation mode */
    pms5003_callbacks_t *cbs;   /**< Registered callbacks */
    pms5003_internal_t internal;        /**< Internal state management */
} pms5003_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Pointer to an PMS5003 device handle
 * @param[in]    params     Initialization parameters
 *
 * @return      0 on success, -1 on error
 */
int pms5003_init(pms5003_t *dev, const pms5003_params_t *params);

/**
 * @brief           Register the given callbacks
 * @param dev       The PMS5003 device to add the callback functions to
 * @param callbacks Functions to call when a measurement completed / an error
 *                  occurred
 */
void pms5003_add_callbacks(pms5003_t *dev, pms5003_callbacks_t *callbacks);

/**
 * @brief           Unregister the given callbacks
 * @param dev       The PMS5003 device to remove the given callbacks from
 * @param callbacks Callbacks to unregister
 */
void pms5003_del_callbacks(pms5003_t *dev, pms5003_callbacks_t *callbacks);

/**
 * @brief       Reset the sensor.
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 *
 * @return      0 on success, -1 on error
 */
void pms5003_reset(pms5003_t *dev);

/**
 * @brief       Set operation mode
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 * @param[in]   mode       Reading mode PMS5003_ACTIVE_MODE or PMS5003_PASSIVE_MODE
 *
 * @return      0 on success, -1 on error
 */
int pms5003_set_mode(pms5003_t *dev, pms5003_mode_t mode);

/**
 * @brief       Get operation mode
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 *
 * @return      operation mode
 */
pms5003_mode_t pms5003_get_mode(pms5003_t *dev);

/**
 * @brief       Set sleep mode
 *
 * @note        The fan warmup period is ~30s, so after waking up from sleep
 *              mode stable data will not be obtained before that
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 * @param[in]   sleep      PMS5003_SLEEP to but device in sleep mode
 *                         PMS5003_RUNNING to wakeup device and set to run
 *
 * @return      0 on success, -1 on error
 */
int pms5003_set_sleep(pms5003_t *dev, pms5003_sleep_t sleep);

/**
 * @brief       Get sleep state
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 *
 * @return      sleep state
 */
pms5003_sleep_t pms5003_get_sleep(pms5003_t *dev);

/**
 * @brief       Read a set of particulate matter measurements.
 *
 * @param[in]   dev        Pointer to an PMS5003 device handle
 * @param[out]  data       Pre-allocated memory to return measurements
 *
 * @return      0 on success, -1 on error
 */
int pms5003_read_measurement(pms5003_t *dev, pms5003_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* PMS5003_H */
/** @} */
