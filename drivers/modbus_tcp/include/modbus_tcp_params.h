/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_modbus_tcp
 *
 * @{
 * @file
 * @brief       Default configuration for Modbus TCP transport
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "net/af.h"
#include "net/sock.h"
#include "time_units.h"

#include "modbus_tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Modbus TCP transport
 * @{
 */
#ifndef MODBUS_TCP_PARAM_MODE
#  define MODBUS_TCP_PARAM_MODE                 MODBUS_TCP_MODE_SERVER
#endif

#ifndef MODBUS_TCP_PARAM_PORT
#  define MODBUS_TCP_PARAM_PORT                 MODBUS_TCP_DEFAULT_PORT
#endif

#ifndef MODBUS_TCP_PARAM_FAMILY
#  ifdef SOCK_HAS_IPV6
#    define MODBUS_TCP_PARAM_FAMILY             AF_INET6
#  else
#    define MODBUS_TCP_PARAM_FAMILY             AF_INET
#  endif
#endif

#ifndef MODBUS_TCP_PARAM_RESPONSE_TIMEOUT
#  define MODBUS_TCP_PARAM_RESPONSE_TIMEOUT     (1U * US_PER_SEC)
#endif

#ifndef MODBUS_TCP_PARAM_IDLE_TIMEOUT
#  define MODBUS_TCP_PARAM_IDLE_TIMEOUT         (0)
#endif

#ifndef MODBUS_TCP_PARAM_ENDPOINT
#  define MODBUS_TCP_PARAM_ENDPOINT \
        { .family = MODBUS_TCP_PARAM_FAMILY, \
          .netif = SOCK_ADDR_ANY_NETIF, \
          .port = MODBUS_TCP_PARAM_PORT }
#endif

#ifndef MODBUS_TCP_PARAMS
#  define MODBUS_TCP_PARAMS \
        { .mode = MODBUS_TCP_PARAM_MODE, \
          .endpoint = MODBUS_TCP_PARAM_ENDPOINT, \
          .response_timeout = MODBUS_TCP_PARAM_RESPONSE_TIMEOUT, \
          .idle_timeout = MODBUS_TCP_PARAM_IDLE_TIMEOUT }
#endif
/** @} */

/**
 * @brief   Configure Modbus TCP transport
 */
static const modbus_tcp_params_t modbus_tcp_params[] =
{
    MODBUS_TCP_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
