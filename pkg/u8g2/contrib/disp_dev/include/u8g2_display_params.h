/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    u8g2_disp_dev_params Default configuration for U8G2 displays
 * @ingroup     u8g2_disp_dev
 *
 * These configuration parameters are used by @ref auto_init when the the integration of
 * @ref pkg_u8g2 to @ref drivers_disp_dev is enabled.
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "board.h"
#include "u8g2_display.h"

#include "periph/i2c.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Default configuration parameters for u8g2 displays
 * @{
 */
/**
 * @brief   Default I2C or SPI device.
 *
 * For I2C displays, set to the desired I2C bus using @ref I2C_DEV (e.g. `I2C_DEV(0)`).
 * For SPI displays, set to the desired SPI bus using @ref SPI_DEV (e.g. `SPI_DEV(0)`).
 */
#ifndef U8G2_DISPLAY_PARAM_DEV
#  define U8G2_DISPLAY_PARAM_DEV        I2C_DEV(0)
#endif

/**
 * @brief   Default I2C address
 * @note    Set to `0` when using SPI
 */
#ifndef U8G2_DISPLAY_PARAM_I2C_ADDR
#  define U8G2_DISPLAY_PARAM_I2C_ADDR       (0x3c)
#endif

/**
 * @brief   Default SPI CS pin
 * @note    Set to @ref GPIO_UNDEF when using I2C
 */
#ifndef U8G2_DISPLAY_PARAM_PIN_CS
#  define U8G2_DISPLAY_PARAM_PIN_CS GPIO_UNDEF
#endif

/**
 * @brief   Default SPI D/C pin
 * @note    Set to @ref GPIO_UNDEF when using I2C
 */
#ifndef U8G2_DISPLAY_PARAM_PIN_DC
#  define U8G2_DISPLAY_PARAM_PIN_DC GPIO_UNDEF
#endif

/**
 * @brief   Default SPI Reset pin
 * @note    Set to @ref GPIO_UNDEF when using I2C
 */
#ifndef U8G2_DISPLAY_PARAM_PIN_RESET
#  define U8G2_DISPLAY_PARAM_PIN_RESET GPIO_UNDEF
#endif

/**
 * @brief   Default U8G2 initialization function
 * @see     Check the u8g2 reference for possible setup functions:
 *          https://github.com/olikraus/u8g2/wiki/u8g2setupc#setup-function-reference
 */
#ifndef U8G2_DISPLAY_PARAM_INIT_FUNCTION
#  define U8G2_DISPLAY_PARAM_INIT_FUNCTION u8g2_Setup_ssd1306_i2c_128x64_noname_f
#endif

/**
 * @brief   Default configuration struct
 */
#ifndef U8G2_DISPLAY_PARAMS
#  define U8G2_DISPLAY_PARAMS \
        {                           \
            .init_function = U8G2_DISPLAY_PARAM_INIT_FUNCTION, \
            .peripheral_configuration = { \
                .device_index = U8G2_DISPLAY_PARAM_DEV, \
                .pin_cs = U8G2_DISPLAY_PARAM_PIN_CS, \
                .pin_dc = U8G2_DISPLAY_PARAM_PIN_DC, \
                .pin_reset = U8G2_DISPLAY_PARAM_PIN_RESET, \
            }, \
            .i2c_address = U8G2_DISPLAY_PARAM_I2C_ADDR, \
        }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const u8g2_display_params_t u8g2_display_params[] =
{
    U8G2_DISPLAY_PARAMS
};

/**
 * @brief   Default screen identifiers
 */
#ifndef U8G2_DISPLAY_PARAM_SCREEN_IDS
#  define U8G2_DISPLAY_PARAM_SCREEN_IDS       0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t u8g2_display_screen_ids[] =
{
    U8G2_DISPLAY_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

/** @} */
