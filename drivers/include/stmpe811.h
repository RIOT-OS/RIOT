/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_stmpe811 STMPE811 Touchscreen Controller
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the STMPE811 touchscreen controller
 *
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_H
#define STMPE811_H

#include "saul.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef MODULE_TOUCH_DEV
#include "touch_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return codes
 */
enum {
    STMPE811_OK = 0,                    /**< Everything was fine */
    STMPE811_ERR_I2C,                   /**< Error on the I2C bus */
    STMPE811_ERR_NODEV,                 /**< No valid device on I2C bus */
    STMPE811_ERR_RESET,                 /**< Software reset failed */
};

/**
 * @brief  Touch state enum
 */
typedef enum {
    STMPE811_TOUCH_STATE_PRESSED,       /**< Touchscreen is pressed */
    STMPE811_TOUCH_STATE_RELEASED,      /**< Touchscreen is released */
} stmpe811_touch_state_t;

/**
 * @brief  Touch position structure
 */
typedef struct {
    uint16_t x;                         /**< X position */
    uint16_t y;                         /**< Y position */
} stmpe811_touch_position_t;

/**
 * @brief   Signature of touch event callback triggered from interrupt
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*touch_event_cb_t)(void *arg);

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c;                         /**< I2C device which is used */
    uint8_t addr;                      /**< Device I2C address */
    gpio_t int_pin;                    /**< Touch screen interrupt pin */
    uint16_t xmax;                     /**< Touch screen max X position */
    uint16_t ymax;                     /**< Touch screen max Y position */
} stmpe811_params_t;

/**
 * @brief   Device descriptor for the STMPE811 sensor
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;                   /**< Pointer to the generic touch device */
#endif
    stmpe811_params_t params;           /**< Device parameters */
    uint16_t prev_x;                    /**< Previous X coordinate */
    uint16_t prev_y;                    /**< Previous Y coordinate */
} stmpe811_t;

/**
 * @brief   Initialize the given STMPE811 device
 *
 * @param[inout] dev        Device descriptor of the STMPE811
 * @param[in] params        Initialization parameters of the STMPE811 device
 * @param[in] cb            Callback function called on touch interrupts
 * @param[in] arg           Context argument used in callback function
 *
 * @return                  STMPE811_OK on success
 * @return                  -STMPE811_ERR_NODEV when no valid device
 * @return                  -STMPE811_ERR_RESET when software reset failed
 * @return                  -STMPE811_ERR_I2C on any I2C error
 */
int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t * params,
                  touch_event_cb_t cb, void *arg);

/**
 * @brief   Read the touch position
 *
 * @param[in] dev           Device descriptor of the STMPE811
 * @param[out] position     Touch position
 *
 * @return                  STMPE811_OK on success
 * @return                  -STMPE811_ERR_I2C on any I2C error
 */
int stmpe811_read_touch_position(stmpe811_t *dev, stmpe811_touch_position_t *position);

/**
 * @brief   Read the touch state (pressed or released)
 *
 * @param[in] dev           Device descriptor of the STMPE811
 * @param[out] state        Touch state
 *
 * @return                  STMPE811_OK on success
 * @return                  -STMPE811_ERR_I2C on any I2C error
 */
int stmpe811_read_touch_state(const stmpe811_t *dev, stmpe811_touch_state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* STMPE811_H */
/** @} */
