/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Default configuration of the Epson ESC/POS driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#include "board.h"
#include "epson_escpos.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default printer profile
 * @{
 */
#ifndef EPSON_ESCPOS_PARAM_PROFILE
#  define EPSON_ESCPOS_PARAM_PROFILE        (&epson_escpos_profile_default)
#endif
/** @} */

#if defined(MODULE_EPSON_ESCPOS_TCP) || defined(DOXYGEN)
/**
 * @name    Default parameters of the TCP transport
 * @{
 */
#ifndef EPSON_ESCPOS_PARAM_TCP_HOST
#  define EPSON_ESCPOS_PARAM_TCP_HOST       "192.168.1.100"
#endif
#ifndef EPSON_ESCPOS_PARAM_TCP_PORT
#  define EPSON_ESCPOS_PARAM_TCP_PORT       (CONFIG_EPSON_ESCPOS_TCP_PORT)
#endif

#ifndef EPSON_ESCPOS_TCP_PARAMS
#  define EPSON_ESCPOS_TCP_PARAMS   { .host = EPSON_ESCPOS_PARAM_TCP_HOST, \
                                      .port = EPSON_ESCPOS_PARAM_TCP_PORT }
#endif
/** @} */

/**
 * @brief   TCP transport configuration
 */
static const epson_escpos_tcp_params_t epson_escpos_tcp_params[] = {
    EPSON_ESCPOS_TCP_PARAMS
};
#endif

#if defined(MODULE_EPSON_ESCPOS_UART) || defined(DOXYGEN)
/**
 * @name    Default parameters of the UART transport
 * @{
 */
#ifndef EPSON_ESCPOS_PARAM_UART_DEV
#  define EPSON_ESCPOS_PARAM_UART_DEV       UART_DEV(1)
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BAUDRATE
#  define EPSON_ESCPOS_PARAM_UART_BAUDRATE  (9600U)
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BUSY_PIN
#  define EPSON_ESCPOS_PARAM_UART_BUSY_PIN  GPIO_UNDEF
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL
#  define EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL    (true)
#endif

#ifndef EPSON_ESCPOS_UART_PARAMS
#  define EPSON_ESCPOS_UART_PARAMS  { .uart = EPSON_ESCPOS_PARAM_UART_DEV,      \
                                      .baudrate = EPSON_ESCPOS_PARAM_UART_BAUDRATE, \
                                      .busy_pin = EPSON_ESCPOS_PARAM_UART_BUSY_PIN, \
                                      .busy_level = EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL }
#endif
/** @} */

/**
 * @brief   UART transport configuration
 */
static const epson_escpos_uart_params_t epson_escpos_uart_params[] = {
    EPSON_ESCPOS_UART_PARAMS
};
#endif

#ifdef __cplusplus
}
#endif

/** @} */
