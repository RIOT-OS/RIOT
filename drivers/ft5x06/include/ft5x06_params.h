/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ft5x06
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef FT5X06_PARAMS_H
#define FT5X06_PARAMS_H

#include <stdint.h>

#include "board.h"
#include "ft5x06.h"
#include "ft5x06_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
/* I2C configuration */
#ifndef FT5X06_PARAM_I2C_DEV
#define FT5X06_PARAM_I2C_DEV            I2C_DEV(0)
#endif
#ifndef FT5X06_PARAM_ADDR
#define FT5X06_PARAM_ADDR               (FT5X06_I2C_DEFAULT_ADDRESS)
#endif
#ifndef FT5X06_PARAM_INT_PIN
#define FT5X06_PARAM_INT_PIN            GPIO_UNDEF
#endif
#ifndef FT5X06_PARAM_XMAX
#define FT5X06_PARAM_XMAX               (480U)
#endif
#ifndef FT5X06_PARAM_YMAX
#define FT5X06_PARAM_YMAX               (272U)
#endif
#ifndef FT5X06_PARAM_XYCONV
#define FT5X06_PARAM_XYCONV             FT5X06_SWAP_XY
#endif
#ifndef FT5X06_PARAM_TYPE
#define FT5X06_PARAM_TYPE               FT5X06_TYPE_FT5336
#endif

#define FT5X06_PARAMS   {               \
    .i2c = FT5X06_PARAM_I2C_DEV,        \
    .addr = FT5X06_PARAM_ADDR,          \
    .int_pin = FT5X06_PARAM_INT_PIN,    \
    .xmax = FT5X06_PARAM_XMAX,          \
    .ymax = FT5X06_PARAM_YMAX,          \
    .xyconv = FT5X06_PARAM_XYCONV,      \
    .type = FT5X06_PARAM_TYPE           \
}
/**@}*/

/**
 * @brief   Configuration struct
 */
static const ft5x06_params_t ft5x06_params[] =
{
    FT5X06_PARAMS
};

/**
 * @brief   Default screen identifiers
 */
#ifndef FT5X06_PARAM_SCREEN_IDS
#define FT5X06_PARAM_SCREEN_IDS         0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t ft5x06_screen_ids[] =
{
    FT5X06_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

#endif /* FT5X06_PARAMS_H */
/** @} */
