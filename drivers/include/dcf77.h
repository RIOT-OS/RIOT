/*
 * Copyright 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_dcf77 DCF77 long wave receiver with 77,5 kHz
 * @ingroup     drivers_sensors
 * @brief       Device driver long wave receiver with 77,5 kHz
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the dcf77 sensor driver
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 */

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "xtimer.h"
#include "time.h"
#include "periph/gpio.h"
#include "dcf77_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* forward-declaration for dcf77_tick_cb_t */
struct dcf77;

/**
 * @brief Signature for tick callback
 *
 * @param[in] dev           device that triggered the alarm
 * @param[in] arg           optional argument to put the callback in the right context
 */
typedef void(*dcf77_tick_cb_t)(struct dcf77 *dev, void *arg);

/**
 * @brief   Possible return codes
 */
enum {
    DCF77_OK          =  0,       /**< all good */
    DCF77_NOCSUM      = -1,       /**< checksum error */
    DCF77_TIMEOUT     = -2,       /**< communication timed out */
    DCF77_INIT_ERROR  = -3        /**< Initialization error */
};

/**
 * @brief   Configuration parameters for DCF77 devices
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin of the device's data pin */
    gpio_mode_t in_mode;    /**< input pin configuration from the device,
                             * without pull resistor */
} dcf77_params_t;

/**
 * @brief   Device descriptor for DCF77 sensor devices
 */
typedef struct dcf77 {
    dcf77_params_t params;      /**< Device parameters */
    dcf77_bits_t bitseq;        /**< contains all Bits from a current cycle */
    dcf77_bits_t last_bitseq;   /**< contains all Bits from a last cycle */
    uint32_t startTime;         /**< Timestamp to measure the term of the level */
    uint8_t internal_state;     /**< internal States  */
    uint8_t bitCounter;         /**< Counter of the Bits in a Bitsequenz */
    dcf77_tick_cb_t tick_cb;    /**< Callback to be called if a new minute starts */
    void *tick_cb_args;         /**< Arguments for the tick callback */
} dcf77_t;

/**
 * @brief   Initialize a new DCF77 device
 *
 * @param[out] dev      device descriptor of a DCF device
 * @param[in]  params   configuration parameters
 *
 * @retval `DCF77_OK`           Success
 * @retval `DCF77_INIT_ERROR`   Error in initialization
 */
int dcf77_init(dcf77_t *dev, const dcf77_params_t *params);

/**
 * @brief   get a new timestamp from the device.
 *
 * @note    if reading fails or checksum is invalid, last_vaules will be unwritten
 *
 * @param[in]  dev          device descriptor of a DCF device
 * @param[in]  time         datastruct for timeinformation
 *
 * @retval `DCF77_OK`         Success
 * @retval `DCF77_NOCSUM`     Checksum error
 */
int dcf77_get_time(dcf77_t *dev, struct tm *time);

/**
 * @brief Set a tick callback for DCF77.
 *
 * The registered callback function will be called for every new minute.
 *
 * @param[in] dev           device descriptor of a DCF device
 * @param[in] cb            Callback executed when a new minute starts.
 * @param[in] arg           Argument passed to callback.
 */
void dcf77_set_tick_cb(dcf77_t *dev, dcf77_tick_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */
