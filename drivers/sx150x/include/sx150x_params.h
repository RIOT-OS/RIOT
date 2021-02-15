/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       Default parameters for SX150x devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SX150X_PARAMS_H
#define SX150X_PARAMS_H

#include "board.h"
#include "sx150x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SX150X
 * @{
 */
#ifndef SX150X_PARAM_I2C_DEV
#define SX150X_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef SX150X_PARAM_I2C_ADDR
#define SX150X_PARAM_I2C_ADDR        SX150X_ADDR_DEFAULT
#endif

#ifndef SX150X_PARAMS
#define SX150X_PARAMS                {{ .bus  = SX150X_PARAM_I2C_DEV,  \
                                       .addr = SX150X_PARAM_I2C_ADDR, }}
#endif

/**
 * References to the SX150x device (or devices, matching @ref SX150X_PARAMS in
 * count) that are to be auto-initialized
 */
#ifndef SX150X_DEVS
#define SX150X_DEVS                  { &board_io_expander }
#endif

/**
 *
 */
#ifndef SX150X_RGBS
#define SX150X_RGBS                  { { &board_io_expander, { 7, 5, 6 } }, \
                                       { &board_io_expander, { 13, 14, 15 } } }
#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* SX150X_PARAMS_H */
/** @} */
