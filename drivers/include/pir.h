/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pir PIR Motion Sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the PIR motion sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the PIR motion sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 */

#ifndef PIR_H
#define PIR_H

#include "sched.h"
#include "periph/gpio.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   PIR specific return values
 */
enum {
    PIR_OK       = 0,     /**< everything went as expected */
    PIR_NOGPIO   = -1,    /**< errors while initializing the GPIO */
    PIR_NOTHREAD = -2,    /**< errors while registering the thread */
    PIR_TIMEERR  = -3,    /**< errors while getting the time information */
};

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    gpio_t gpio;      /**< GPIO device which is used */
    bool active_high; /**< Active when GPIO pin is high or not */
} pir_params_t;

/**
 * @brief   device descriptor for a PIR sensor
 */
typedef struct {
    uint64_t start_active_time;  /**< Time when PIR starts to be active */
    uint64_t accum_active_time;  /**< Accumulated active time */
    uint64_t last_read_time;     /**< Last time when PIR status is read */
    kernel_pid_t msg_thread_pid; /**< thread to msg on irq */
    bool active;                 /**< Indicate PIR is active or not */
    pir_params_t p;              /**< Configuration parameters */
} pir_t;

/**
 * @defgroup drivers_pir_config     PIR Motion Sensor driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Sensitivity of sensor
 */
#ifndef PIR_MSG_T_STATUS_START
#define PIR_MSG_T_STATUS_START 150
#endif
/** @} */

/**
 * @brief   event type for a PIR sensor
 */
typedef enum {
    PIR_STATUS_ACTIVE = PIR_MSG_T_STATUS_START,   /**< motion was detected */
    PIR_STATUS_INACTIVE,                          /**< no motion is detected */
} pir_event_t;

/**
 * @brief   Initialize a PIR motion sensor
 *
 * The PIR motion sensor is interfaced by a single GPIO pin, specified by
 * `gpio`.
 *
 * @note
 * The sensor needs up to a minute to settle down before meaningful
 * measurements can be made.
 *
 * @param[out] dev      device descriptor of an PIR sensor
 * @param[in] params    parameters of the PIR sensor
 *
 * @return              0 on success
 * @return              -1 on error
 */
int pir_init(pir_t *dev, const pir_params_t* params);

/**
 * @brief   Read the current status of the motion sensor
 *
 * @param[in] dev       device descriptor of the PIR motion sensor to read from
 *
 * @return              1 if motion is detected, 0 otherwise
 */
pir_event_t pir_get_status(const pir_t *dev);

/**
 * @brief   Read OCCUPANCY value
 *
 * @param[in] dev       device descriptor of the PIR motion sensor to read from
 * @param[out] occup    occupancy ratio [in 100 * percentage]
 *                      The value is renewed when it is read. So it is percentage
 *                      of occupancy since the last read.
 *
 * @return              0 on success,
 * @return              -1 on errors,
 */
int pir_get_occupancy(pir_t *dev, int16_t *occup);

/**
 * @brief   Register a thread for notification when state changes on the
 *          motion sensor.
 *
 * @note
 * This configures the gpio device for interrupt driven operation.
 *
 * @param[in] dev       device descriptor of the PIR motion sensor to
 *                      register for
 *
 * @return              0 on succuess,
 * @return              -1 on internal errors,
 * @return              -2 if another thread is registered already
 */
int pir_register_thread(pir_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* PIR_H */
/** @} */
