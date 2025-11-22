/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include "saul.h"
#include "periph/gpio.h"
#if IS_USED(MODULE_STMPE811_SPI)
#include "periph/spi.h"
#else
#include "periph/i2c.h"
#endif

#ifdef MODULE_TOUCH_DEV
#include "touch_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Touch state enum
 */
typedef enum {
    STMPE811_TOUCH_STATE_PRESSED,       /**< Touchscreen is pressed */
    STMPE811_TOUCH_STATE_RELEASED,      /**< Touchscreen is released */
} stmpe811_touch_state_t;

/**
 * @brief   Touch screen coordinate conversions
 *
 * Normally the coordinates of the touch device must be converted to the
 * screen coordinates by swapping and/or mirroring. The flags defined by
 * this enumeration can be ORed for a combined conversion. In this case,
 * the swapping is performed before the mirroring.
 *
 * @note The maximum X and Y screen coordinates defined by
 *       @ref stmpe811_params_t::xmax and @ref stmpe811_params_t::ymax
 *       define the dimension of the touch device in screen coordinates,
 *       i.e. after conversion.
 */
typedef enum {
    STMPE811_NO_CONV  = 0x00, /**< No conversion */
    STMPE811_MIRROR_X = 0x01, /**< Mirror X, applied after optional swapping */
    STMPE811_MIRROR_Y = 0x02, /**< Mirror Y, applied after optional swapping */
    STMPE811_SWAP_XY  = 0x04, /**< Swap XY, applied before optional mirroring */
} stmpe811_touch_conv_t;

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
typedef void (*stmpe811_event_cb_t)(void *arg);

/**
 * @brief   Device initialization parameters
 *
 * @note stmpe811_params_t::xmax and stmpe811_params_t::ymax define the
 *       maximum X and Y values in screen coordinates after the optional
 *       conversion defined by stmpe811_params_t::xmax.
 */
typedef struct {
#if IS_USED(MODULE_STMPE811_SPI)
    /* SPI configuration */
    spi_t spi;                          /**< SPI bus */
    spi_mode_t mode;                    /**< SPI mode */
    spi_clk_t clk;                      /**< clock speed for the SPI bus */
    gpio_t cs;                          /**< chip select pin */
#else
    /* I2C details */
    i2c_t i2c;                         /**< I2C device which is used */
    uint8_t addr;                      /**< Device I2C address */
#endif
    gpio_t int_pin;                    /**< Touch screen interrupt pin */
    uint16_t xmax;                     /**< Touch screen max X position */
    uint16_t ymax;                     /**< Touch screen max Y position */
    stmpe811_touch_conv_t xyconv;      /**< Touch screen coordinates conversion,
                                            swapping is applied before mirroring */
} stmpe811_params_t;

/**
 * @brief   Device descriptor for the STMPE811 sensor
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;                   /**< Pointer to the generic touch device */
#endif
    stmpe811_params_t params;           /**< Device parameters */
    stmpe811_event_cb_t cb;             /**< Configured IRQ event callback */
    void *cb_arg;                       /**< Extra argument for the callback */
    uint16_t prev_x;                    /**< Previous X coordinate */
    uint16_t prev_y;                    /**< Previous Y coordinate */
} stmpe811_t;

/**
 * @brief   Initialize the given STMPE811 device
 *
 * @param[in,out] dev       Device descriptor of the STMPE811
 * @param[in]     params    Initialization parameters of the STMPE811 device
 * @param[in]     cb        Callback function called on touch interrupts
 * @param[in]     arg       Context argument used in callback function
 *
 * @return                  0 on success
 * @return                  -ENODEV when no valid device
 * @return                  -EIO when software reset failed
 * @return                  -EPROTO on any bus error
 */
int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t * params,
                  stmpe811_event_cb_t cb, void *arg);

/**
 * @brief   Read the touch position
 *
 * @param[in] dev           Device descriptor of the STMPE811
 * @param[out] position     Touch position
 *
 * @return                  0 on success
 * @return                  -EPROTO on any bus error
 */
int stmpe811_read_touch_position(stmpe811_t *dev, stmpe811_touch_position_t *position);

/**
 * @brief   Read the touch state (pressed or released)
 *
 * @param[in] dev           Device descriptor of the STMPE811
 * @param[out] state        Touch state
 *
 * @return                  0 on success
 * @return                  -EPROTO on any busI2aC error
 */
int stmpe811_read_touch_state(const stmpe811_t *dev, stmpe811_touch_state_t *state);

#ifdef __cplusplus
}
#endif

/** @} */
