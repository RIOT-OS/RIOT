/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ft5x06 FocalTech FT5x06 touch panel driver
 * @ingroup     drivers_sensors
 * @brief       Multi-touch capacitive panel controller
 *
 * This driver should also support FT5x06，FT5606，FT5x16，FT6x06，Ft6x36
 * FT5x06i，FT5336，FT3316，FT5436i，FT5336i，FT5x46.
 *
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef FT5X06_H
#define FT5X06_H

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#include "ft5x06_constants.h"

#ifdef MODULE_TOUCH_DEV
#include "touch_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Touch position structure
 */
typedef struct {
    uint16_t x;                         /**< X position */
    uint16_t y;                         /**< Y position */
} ft5x06_touch_position_t;

/**
 * @brief  Touch gesture
 */
typedef enum {
    FT5X06_TOUCH_NO_GESTURE,            /**< No gesture detected */
    FT5X06_TOUCH_MOVE_UP,               /**< Move up gesture detected */
    FT5X06_TOUCH_MOVE_LEFT,             /**< Move left gesture detected */
    FT5X06_TOUCH_MOVE_DOWN,             /**< Move down gesture detected */
    FT5X06_TOUCH_MOVE_RIGHT,            /**< Move right gesture detected */
    FT5X06_TOUCH_ZOOM_IN,               /**< Zoom int gesture detected */
    FT5X06_TOUCH_ZOOM_OUT,              /**< Zoom out gesture detected */
} ft5x06_touch_gesture_t;

/**
 * @brief   Signature of the touch event callback triggered from interrupt
 *
 * @param[in] arg           optional context for the callback
 */
typedef void (*ft5x06_event_cb_t)(void *arg);

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C device which is used */
    uint8_t addr;           /**< Device I2C address */
    gpio_t int_pin;         /**< Touch screen interrupt pin */
    uint16_t xmax;          /**< Touch screen max X position */
    uint16_t ymax;          /**< Touch screen max Y position */
} ft5x06_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;       /**< Pointer to the generic touch device */
#endif
    ft5x06_params_t params; /**< Initialization parameters */
} ft5x06_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in] params        Initialization parameters
 * @param[in] cb            Callback function called on touch interrupts
 * @param[in] arg           Context argument used in callback function
 *
 * @return                  0 on success
 * @return                  -ENODEV when no valid device
 * @return                  -EPROTO on any bus error
 */
int ft5x06_init(ft5x06_t *dev, const ft5x06_params_t *params,
                ft5x06_event_cb_t cb, void *arg);

/**
 * @brief   Read the touch positions
 *
 * @param[in] dev           Device descriptor of the FT5x06
 * @param[out] positions    Touch positions
 * @param[in] len           Number of touch positions to read
 *
 * @return                  0 on success
 * @return                  -EPROTO on any bus error
 */
int ft5x06_read_touch_positions(const ft5x06_t *dev, ft5x06_touch_position_t *positions, size_t len);

/**
 * @brief   Read the number of touch
 *
 * @param[in] dev           Device descriptor of the FT5x06
 * @param[out] count        Number of touch detected
 *
 * @return                  0 on success
 * @return                  -EPROTO on any bus error
 */
int ft5x06_read_touch_count(const ft5x06_t *dev, uint8_t *count);

/**
 * @brief   Read the gesture detected
 *
 * @param[in] dev           Device descriptor of the FT5X06
 * @param[out] gesture      Gesture ID
 *
 * @return                  0 on success
 * @return                  -EPROTO on any bus error
 */
int ft5x06_read_touch_gesture(const ft5x06_t *dev, ft5x06_touch_gesture_t *gesture);

#ifdef __cplusplus
}
#endif

#endif /* FT5X06_H */
/** @} */
