/*
 * Copyright (C) 2019 Noel Le Moult <noel.lemoult@dfxlab.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tca6507
 * @brief       Default configuration for the TCA6507 I2C Led controller
 * @author      Noel Le Moult <noel.lemoult@dfxlab.fr>
 * @author      William MARTIN <william.martin@power-lan.com>
 * @file
 * @{
 */

#ifndef TCA6507_PARAMS_H
#define TCA6507_PARAMS_H

#include "board.h"
#include "tca6507.h"
#include "tca6507_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TCA6507 sensor
 * @{
 */
#ifndef TCA6507_PARAM_I2C_DEV
#define TCA6507_PARAM_I2C_DEV         I2C_DEV(0)
#endif

#ifndef TCA6507_PARAM_ADDR
#define TCA6507_PARAM_ADDR            TCA6507_I2C_ADDRESS
#endif

#ifndef TCA6507_PARAMS
#define TCA6507_PARAMS                { .i2c_dev = TCA6507_PARAM_I2C_DEV, \
                                        .address = TCA6507_PARAM_ADDR }
#endif
/**@}*/

/**
 * @brief   Configure TCA6507
 */
static const tca6507_params_t tca6507_params[] =
{
    TCA6507_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* TCA6507_PARAMS_H */
/** @} */
