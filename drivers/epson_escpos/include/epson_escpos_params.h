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
/** @brief printer profile */
#  define EPSON_ESCPOS_PARAM_PROFILE        (&epson_escpos_profile_default)
#endif
/** @} */

#if defined(MODULE_EPSON_ESCPOS_TCP) || defined(DOXYGEN)
/**
 * @name    Default parameters of the TCP transport
 * @{
 */
#ifndef EPSON_ESCPOS_PARAM_TCP_HOST
/** @brief IPv4 address of the printer */
#  define EPSON_ESCPOS_PARAM_TCP_HOST       CONFIG_EPSON_ESCPOS_TCP_HOST
#endif
#ifndef EPSON_ESCPOS_PARAM_TCP_PORT
/** @brief TCP port of the printer */
#  define EPSON_ESCPOS_PARAM_TCP_PORT       (CONFIG_EPSON_ESCPOS_TCP_PORT)
#endif

#ifndef EPSON_ESCPOS_TCP_PARAMS
/** @brief TCP transport parameters */
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
/** @brief UART device */
#  define EPSON_ESCPOS_PARAM_UART_DEV       UART_DEV(CONFIG_EPSON_ESCPOS_UART_DEV)
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BAUDRATE
/** @brief baud rate */
#  define EPSON_ESCPOS_PARAM_UART_BAUDRATE  (CONFIG_EPSON_ESCPOS_UART_BAUDRATE)
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BUSY_PIN
/** @brief BUSY input, GPIO_UNDEF if not used */
#  define EPSON_ESCPOS_PARAM_UART_BUSY_PIN  GPIO_UNDEF
#endif
#ifndef EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL
/** @brief level of the BUSY input while busy */
#  define EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL    (true)
#endif

#ifndef EPSON_ESCPOS_UART_PARAMS
/** @brief UART transport parameters */
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
