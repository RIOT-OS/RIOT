/*
 * Copyright (C) 2018-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "stdio_uart.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for slipmux driver
 * @{
 */
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
    SLIPMUX_PARAMS, { .uart = UART_DEV(1), .baudrate = SLIPMUX_PARAM_BAUDRATE}
};

#ifdef __cplusplus
}
#endif

/** @} */
