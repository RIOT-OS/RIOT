/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_aip31068
 * @brief       Default configuration for the AIP31068 I2C LCD controller
 *
 * @{
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */
#ifndef AIP31068_PARAMS_H
#define AIP31068_PARAMS_H

#include <stdbool.h>

#include "periph/i2c.h"

#include "aip31068_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @name    Set default configuration parameters
* @{
*/
#ifndef AIP31068_PARAM_I2C_DEV
/** I2C device is I2C_DEV(0) */
#define AIP31068_PARAM_I2C_DEV               I2C_DEV(0)
#endif

#ifndef AIP31068_PARAM_I2C_ADDR
/** I2C address of device is (0x7c >> 1) */
#define AIP31068_PARAM_I2C_ADDR              (0x7c >> 1)
#endif

#ifndef AIP31068_PARAMS
#define AIP31068_PARAMS                         \
    {                                           \
        .i2c_dev = AIP31068_PARAM_I2C_DEV,      \
        .i2c_addr = AIP31068_PARAM_I2C_ADDR,    \
        .row_count = 2,                         \
        .col_count = 16,                        \
        .font_size = FONT_SIZE_5x8,             \
        .bit_mode = BITMODE_8_BIT,              \
    }
#endif /* AIP31068_PARAMS */
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const aip31068_params_t aip31068_params[] =
{
        AIP31068_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* AIP31068_PARAMS_H */
/** @} */
