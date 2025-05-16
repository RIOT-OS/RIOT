/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
 * @brief  Device type
 */
typedef enum {
    FT5X06_TYPE_FT5X06,                 /**< FT5X06 */
    FT5X06_TYPE_FT5606,                 /**< FT5606 */
    FT5X06_TYPE_FT5X16,                 /**< FT5X16 */
    FT5X06_TYPE_FT6X06,                 /**< FT6X06 */
    FT5X06_TYPE_FT6X36,                 /**< FT6X36 */
    FT5X06_TYPE_FT5X06I,                /**< FT5X06I */
    FT5X06_TYPE_FT5336,                 /**< FT5336 */
    FT5X06_TYPE_FT3316,                 /**< FT3316 */
    FT5X06_TYPE_FT5436I,                /**< FT5436I */
    FT5X06_TYPE_FT5336I,                /**< FT5336I */
    FT5X06_TYPE_FT5X46,                 /**< FT5X46 */
} ft5x06_type_t;

/**
 * @brief   Touch screen coordinate conversions
 *
 * Normally the coordinates of the touch device must be converted to the
 * screen coordinates by swapping and/or mirroring. The flags defined by
 * this enumeration can be ORed for a combined conversion. In this case,
 * the swapping is performed before the mirroring.
 *
 * @note The maximum X and Y screen coordinates defined by
 *       @ref ft5x06_params_t::xmax and @ref ft5x06_params_t::ymax
 *       define the dimension of the touch device in screen coordinates,
 *       i.e. after conversion.
 */
typedef enum {
    FT5X06_NO_CONV  = 0x00, /**< No conversion */
    FT5X06_MIRROR_X = 0x01, /**< Mirror X, applied after optional swapping */
    FT5X06_MIRROR_Y = 0x02, /**< Mirror Y, applied after optional swapping */
    FT5X06_SWAP_XY  = 0x04, /**< Swap XY, applied before optional mirroring */
} ft5x06_touch_conv_t;

/**
 * @brief   Signature of the touch event callback triggered from interrupt
 *
 * @param[in] arg           optional context for the callback
 */
typedef void (*ft5x06_event_cb_t)(void *arg);

/**
 * @brief   Device initialization parameters
 *
 * @note ft5x06_params_t::xmax and ft5x06_params_t::ymax define the
 *       maximum X and Y values in screen coordinates after the optional
 *       conversion defined by ft5x06_params_t::xmax.
 */
typedef struct {
    i2c_t i2c;                  /**< I2C device which is used */
    uint8_t addr;               /**< Device I2C address */
    gpio_t int_pin;             /**< Touch screen interrupt pin */
    uint16_t xmax;              /**< Touch screen max X position */
    uint16_t ymax;              /**< Touch screen max Y position */
    ft5x06_touch_conv_t xyconv; /**< Touch screen coordinates conversion,
                                     swapping is performed before mirroring */
    ft5x06_type_t type;         /**< Device type */
} ft5x06_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;               /**< Pointer to the generic touch device */
#endif
    const ft5x06_params_t *params;  /**< Initialization parameters */
} ft5x06_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 * @param[in]     cb        Callback function called on touch interrupts
 * @param[in]     arg       Context argument used in callback function
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

/** @} */
