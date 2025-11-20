/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_tsl4531x TSL4531x Illuminance sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver for the AMS TSL4531 sensor
 * @{
 *
 * # Power modes
 *
 * This driver has two power modes: high and low. Its startup mode is configured
 * in the initialization parameters, and it can also be changed during runtime.
 *
 * In high power mode, the user application can read from the device using the
 * tsl4531x_simple_read function, and it will return immediately.
 *
 * In low power mode, the user application can interact with the driver in a
 * synchronous or asynchronous manner. For synchronous operation, the application
 * can call tsl4531x_simple_read, and the driver will block for the integration
 * time defined in the initialization parameters (plus a small margin, to encompass
 * the max times indicated in the datasheet). For asynchronous operation, the
 * application needs to use the functions tsl4531x_start_sample,
 * tsl4531x_time_until_sample_ready and tsl4531x_get_sample, as described in those
 * functions' descriptions.
 *
 * Both modes will work through SAUL, with the low-power mode being synchronous.
 *
 * @file
 * @brief       Device driver for the AMS TSL4531 sensor
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 * @author      Daniel Petry <daniel.petry@fu-berlin.de>
 */

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Integration times
 */
typedef enum {
    TSL4531X_INTEGRATE_400MS = 0,       /* 0b00 */
    TSL4531X_INTEGRATE_200MS = 1,       /* 0b01 */
    TSL4531X_INTEGRATE_100MS = 2,       /* 0b10 */
}tsl4531x_intgn_time_t;

/**
 * @name  Fixed values for different members of the TSL4531x series
 * @{
 *
 * @brief Part numbers
 */
#define TSL45311_PARTNO                  (0x8)
#define TSL45313_PARTNO                  (0x9)
#define TSL45315_PARTNO                  (0xA)
#define TSL45317_PARTNO                  (0xB)

/**
 * @brief TSL4531x I2C addresses
 */
#define TSL45311_ADDR                    (0x39)
#define TSL45313_ADDR                    (0x39)
#define TSL45315_ADDR                    (0x29)
#define TSL45317_ADDR                    (0x29)
/** @} */

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                              /**< I2C device which is used */
    i2c_t i2c_addr;                             /**< I2C address of sensor */
    tsl4531x_intgn_time_t integration_time;     /**< integration time */
    uint8_t low_power_mode : 1;                 /**< low power mode */
    uint8_t part_number;                        /**< part number, according to variant */
} tsl4531x_params_t;

/**
 * @brief Device descriptor
 */
typedef struct {
    i2c_t i2c_dev;                              /**< I2C device which is used */
    i2c_t i2c_addr;                             /**< I2C address of sensor */
    tsl4531x_intgn_time_t integration_time;     /**< integration time */
    uint8_t low_power_mode : 1;                 /**< low power mode */
    uint8_t high_power_mode_started_up : 1;     /**< high power mode started up flag */
    uint32_t sample_start_time;                 /**< sample start time */
} tsl4531x_t;

/**
 * Initialize the TSL4531x device.
 *
 * @param[out] dev        Initialized device descriptor
 * @param[in]  params     Device initialization parameters
 *
 * @return     Zero on success
 * @return     -ENODEV if I2C bus can't be acquired
 * @return     -ENXIO if device can't be read or configured
 * @return     -ENOTSUP if ID, once read, is wrong
 */
int tsl4531x_init(tsl4531x_t *dev, const tsl4531x_params_t *params);

/**
 * Set the low power mode of the driver on or off.
 *
 * @param      dev           Initialized device descriptor
 * @param      low_power_on  Bool indicating whether low power mode is on or off
 *
 * @return     Zero
 */
int tsl4531x_set_low_power_mode(tsl4531x_t *dev, uint8_t low_power_on);

/**
 * Start collecting sample in low power mode. This provides asynchronous operation
 * along with tsl4531x_time_until_sample_ready and tsl4531x_get_sample. It does
 * nothing in high power mode.
 *
 * @param      dev           Initialized device descriptor
 *
 * @return     Zero
 */
int tsl4531x_start_sample(tsl4531x_t *dev);

/**
 * Deliver time in microseconds until sample is ready, or zero if it is ready.
 * In low power mode, this counts down from the point at which tsl4531x_start_sample
 * is called, and along with that function and tsl4531x_get_sample, provides
 * asynchronous operation. In high power mode, this counts down from the point
 * at which the driver is switched into high power mode or started up, and
 * indicates whether enough time has passed for a full sample to be collected.
 *
 * Note that for low power mode this rolls over and repeats its behaviour every
 * 1.2 hours. The sample should have been collected well before this, however.
 *
 * The countdown time equals the integration time, which can be set in the
 * device initialisation parameters, plus 5% margin to encompass the max times
 * indicated in the datasheet.
 *
 * @param      dev           Initialized device descriptor
 *
 * @return    Time in microseconds until sample is ready
 */
uint32_t  tsl4531x_time_until_sample_ready(tsl4531x_t *dev);

/**
 * Reads the sample from the device immediately. In high power mode, this does
 * the same as tsl4531x_simple_read once the device has performed one
 * integration cycle. In low power mode, this provides asynchronous operation
 * along with tsl4531x_start_sample and tsl4531x_time_until_sample_ready which
 * determine whether the device has performed an integration cycle.
 *
 * Note that this function will always return the value stored in the device's
 * internal register, and this value will be sensible physically, representing
 * the last time an integration cycle has been performed. However, in order for
 * it to be accurate, the start_sample and time_until_sample_ready functions
 * need to also be used, or alternatively the simple_read function can be used.
 *
 * @param      dev           Initialized device descriptor
 *
 * @return     The sample value in Lux
 */
int tsl4531x_get_sample(const tsl4531x_t *dev);

/**
 * Reads the sample from the device. In low power mode, or in high power mode
 * just after startup, this starts collecting the sample, blocks until the
 * sample is ready (400/200/100ms depending on the integration time set in the
 * initialization parameters), and then reads and returns the sample.
 *
 * @param      dev           Initialized device descriptor
 *
 * @return     The sample value in Lux
 */
int tsl4531x_simple_read(tsl4531x_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
