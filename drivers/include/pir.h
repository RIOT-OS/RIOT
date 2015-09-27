/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_pir PIR Motion Sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the PIR motion sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the PIR motion sensor
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef PIR_H
#define PIR_H

#include "kernel_types.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief device descriptor for a PIR sensor
 */
typedef struct {
    gpio_t          gpio_dev;       /**< GPIO device which is used */
    kernel_pid_t    msg_thread_pid; /**< thread to msg on irq */
} pir_t;

#ifndef PIR_MSG_T_STATUS_START
#define PIR_MSG_T_STATUS_START 150
#endif

typedef enum {
    PIR_STATUS_HI = PIR_MSG_T_STATUS_START,     /**< motion was detected */
    PIR_STATUS_LO,                              /**< no motion is detected */
} pir_event_t;

/**
 * @brief               Initialize a PIR motion sensor
 *
 * The PIR motion sensor is interfaced by a single GPIO pin, specified by
 * `gpio`.
 *
 * @note
 * The sensor needs up to a minute to settle down before meaningful
 * measurements can be made.
 *
 * @param[out] dev      device descriptor of an PIR sensor
 * @param[in] gpio      the GPIO device the sensor is connected to
 *
 * @return              0 on success
 * @return              -1 on error
 */
int pir_init(pir_t *dev, gpio_t gpio);

/**
 * @brief               Read the current status of the motion sensor
 *
 * @param[in] dev       device descriptor of the PIR motion sensor to read from
 *
 * @return              1 if motion is detected, 0 otherwise
 */
pir_event_t pir_get_status(pir_t *dev);

/**
 * @brief   Register a thread for notification whan state changes on the
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
