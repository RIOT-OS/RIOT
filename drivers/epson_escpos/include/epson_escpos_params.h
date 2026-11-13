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
/** @brief printer profile */
#ifndef EPSON_ESCPOS_PARAM_PROFILE
#  define EPSON_ESCPOS_PARAM_PROFILE        (&epson_escpos_profile_tm_t88iv)
#endif
/** @} */

#if defined(MODULE_EPSON_ESCPOS_TCP) || defined(DOXYGEN)
/**
 * @name    Default parameters of the TCP transport
 *
 * The address and port of the printer are configured with
 * @ref CONFIG_EPSON_ESCPOS_TCP_HOST and @ref CONFIG_EPSON_ESCPOS_TCP_PORT.
 * @{
 */
/** @brief TCP transport parameters */
#  ifndef EPSON_ESCPOS_TCP_PARAMS
#    define EPSON_ESCPOS_TCP_PARAMS     { .host = CONFIG_EPSON_ESCPOS_TCP_HOST, \
                                          .port = CONFIG_EPSON_ESCPOS_TCP_PORT }
#  endif
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
 *
 * The UART device and baud rate are configured with
 * @ref CONFIG_EPSON_ESCPOS_UART_DEV and @ref CONFIG_EPSON_ESCPOS_UART_BAUDRATE.
 * @{
 */
/** @brief BUSY input, GPIO_UNDEF if not used */
#  ifndef EPSON_ESCPOS_PARAM_UART_BUSY_PIN
#    define EPSON_ESCPOS_PARAM_UART_BUSY_PIN    GPIO_UNDEF
#  endif
/** @brief level of the BUSY input while busy */
#  ifndef EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL
#    define EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL  (true)
#  endif

#  ifndef EPSON_ESCPOS_UART_PARAMS
/** @brief UART transport parameters */
#    define EPSON_ESCPOS_UART_PARAMS    { .uart = UART_DEV(CONFIG_EPSON_ESCPOS_UART_DEV), \
                                          .baudrate = CONFIG_EPSON_ESCPOS_UART_BAUDRATE,  \
                                          .busy_pin = EPSON_ESCPOS_PARAM_UART_BUSY_PIN,   \
                                          .busy_level = EPSON_ESCPOS_PARAM_UART_BUSY_LEVEL }
#  endif
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
