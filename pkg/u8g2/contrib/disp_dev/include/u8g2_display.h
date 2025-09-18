/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    u8g2_disp_dev Display device generic API implementation for U8G2
 * @ingroup     pkg_u8g2
 * @brief       Implementation of display device generic API for U8G2 monochrome displays
 *
 * For more information on how to use this module, refer to @ref pkg_u8g2.
 *
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <inttypes.h>

#include "u8g2.h"
#include "u8x8_riotos.h"

#include "disp_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Function pointer type for u8g2 initialization functions
 * @see Check the u8g2 reference for possible setup functions:
 *      https://github.com/olikraus/u8g2/wiki/u8g2setupc#setup-function-reference
 */
typedef void (*u8g2_init_function_t)(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb,
                                     u8x8_msg_cb gpio_and_delay_cb);

/**
 * @brief   U8G2 display initialization parameters
 */
typedef struct {
    u8g2_init_function_t init_function;         /**< Initialization function for u8g2 */
    u8x8_riotos_t peripheral_configuration;     /**< Peripheral configuration for RIOT-OS */
    /**
     * I2C address of the display. Set to 0 when using SPI.
     */
    uint8_t i2c_address;
} u8g2_display_params_t;

/**
 * @brief   U8G2 display device structure
 */
typedef struct {
    disp_dev_t *dev;                /**< Pointer to disp_dev instance (@ref drivers_disp_dev) */
    u8g2_display_params_t params;   /**< Device initialization parameters */
    u8g2_t u8g2;                    /**< U8G2 instance (@ref pkg_u8g2) */
} u8g2_display_t;

/**
 * @brief   Initialize a monochrome u8g2 display device
 *
 * @param[in,out] dev   Device descriptor of the driver
 * @param[in] params    Initialization parameters
 *
 * @retval  0 on success
 * @retval  -1 on error
 *
 * @pre @p dev must not be `NULL`
 * @pre @p params must not be `NULL`
 */
int u8g2_display_init(u8g2_display_t *dev, const u8g2_display_params_t *params);

#ifdef __cplusplus
}
#endif

/** @} */
