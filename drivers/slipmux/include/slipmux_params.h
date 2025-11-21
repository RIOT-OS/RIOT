/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_slipmux
 * @{
 *
 * @file
 * @brief   Default configuration for the SLIPMUX device driver
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board.h"
#include "slipmux.h"
#ifdef MODULE_SLIPMUX_STDIO
#  include "stdio_uart.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for slipmux driver
 * @{
 */
#if DOXYGEN
/**
 * @brief   Default UART to use
 */
#define SLIPMUX_PARAM_UART          UART_DEV(0)
/**
 * @brief   Default baudrate to use
 */
#define SLIPMUX_PARAM_BAUDRATE      (115200U)
#endif /* DOXYGEN */

#ifndef SLIPMUX_PARAM_UART
# ifndef MODULE_SLIPMUX_STDIO
#  ifdef MODULE_USBUS_CDC_ACM
#   define SLIPMUX_PARAM_UART       UART_DEV(0)
#  else
#   define SLIPMUX_PARAM_UART       UART_DEV(1)
#  endif
# else  /* MODULE_SLIPMUX_STDIO */
#  define SLIPMUX_PARAM_UART        STDIO_UART_DEV
# endif /* MODULE_SLIPMUX_STDIO */
#endif  /* SLIPMUX_PARAM_UART */
#ifndef SLIPMUX_PARAM_BAUDRATE
# ifndef MODULE_SLIPMUX_STDIO
#  define SLIPMUX_PARAM_BAUDRATE    (115200U)
# else  /* MODULE_SLIPMUX_STDIO */
#  define SLIPMUX_PARAM_BAUDRATE    (STDIO_UART_BAUDRATE)
# endif /* MODULE_SLIPMUX_STDIO */
#endif  /* SLIPMUX_PARAM_BAUDRATE */

#ifndef SLIPMUX_PARAMS
/**
 * @brief   Default UART parameters for SLIPMUX
 */
#define SLIPMUX_PARAMS          { .uart = SLIPMUX_PARAM_UART,   \
                                  .baudrate = SLIPMUX_PARAM_BAUDRATE }
#endif
/** @} */

/**
 * @brief   slipmux configuration
 *
 * The first element in this array will be used to multiplex stdio if
 * `slipmux_stdio` is included.
 */
static const slipmux_params_t slipmux_params[] = {
    SLIPMUX_PARAMS,
};

#ifdef __cplusplus
}
#endif

/** @} */
