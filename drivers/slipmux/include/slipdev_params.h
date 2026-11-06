/*
 * SPDX-FileCopyrightText: 2018-2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_slipdev
 * @{
 *
 * @file
 * @brief   Default configuration for the SLIP device driver
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board.h"
#include "slipdev.h"
#include "stdio_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for slipdev driver
 * @{
 */
#ifndef SLIPDEV_PARAM_UART
# ifdef MODULE_STDIO_UART
#  define SLIPDEV_PARAM_UART        UART_DEV(1)
# else
#  define SLIPDEV_PARAM_UART        STDIO_UART_DEV
# endif
#endif  /* SLIPDEV_PARAM_UART */

#ifndef SLIPDEV_PARAM_BAUDRATE
# define SLIPDEV_PARAM_BAUDRATE     STDIO_UART_BAUDRATE
#endif  /* SLIPDEV_PARAM_BAUDRATE */

#ifndef SLIPDEV_PARAMS
#define SLIPDEV_PARAMS          { .uart = SLIPDEV_PARAM_UART,   \
                                  .baudrate = SLIPDEV_PARAM_BAUDRATE }
#endif
/** @} */

/**
 * @brief   slipdev configuration
 *
 * The first element in this array will be used to multiplex stdio if
 * `stdio_slipdev` is included.
 */
static const slipdev_params_t slipdev_params[] = {
    SLIPDEV_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
