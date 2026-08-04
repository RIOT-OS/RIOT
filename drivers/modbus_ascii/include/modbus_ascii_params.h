/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_modbus_ascii
 *
 * @{
 * @file
 * @brief       Default configuration for Modbus ASCII transport
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"
#include "time_units.h"

#include "modbus_ascii.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Modbus ASCII transport
 * @{
 */
#ifndef MODBUS_ASCII_PARAM_PIN_RTS
#  define MODBUS_ASCII_PARAM_PIN_RTS            GPIO_UNDEF
#endif

#ifndef MODBUS_ASCII_PARAM_PIN_RTS_ENABLE
#  define MODBUS_ASCII_PARAM_PIN_RTS_ENABLE     (true)
#endif

#ifndef MODBUS_ASCII_PARAM_UART_DEV
#  define MODBUS_ASCII_PARAM_UART_DEV           UART_DEV(1)
#endif

#ifndef MODBUS_ASCII_PARAM_UART_BAUDRATE
#  define MODBUS_ASCII_PARAM_UART_BAUDRATE      (9600)
#endif

#ifndef MODBUS_ASCII_PARAM_RESPONSE_TIMEOUT
#  define MODBUS_ASCII_PARAM_RESPONSE_TIMEOUT   (1U * US_PER_SEC)
#endif

#ifndef MODBUS_ASCII_PARAM_CHAR_TIMEOUT
#  define MODBUS_ASCII_PARAM_CHAR_TIMEOUT       (1U * US_PER_SEC)
#endif

#ifndef MODBUS_ASCII_PARAMS
#  define MODBUS_ASCII_PARAMS \
        { .pin_rts = MODBUS_ASCII_PARAM_PIN_RTS, \
          .pin_rts_enable = MODBUS_ASCII_PARAM_PIN_RTS_ENABLE, \
          .uart = MODBUS_ASCII_PARAM_UART_DEV, \
          .baudrate = MODBUS_ASCII_PARAM_UART_BAUDRATE, \
          .response_timeout = MODBUS_ASCII_PARAM_RESPONSE_TIMEOUT, \
          .char_timeout = MODBUS_ASCII_PARAM_CHAR_TIMEOUT }
#endif
/** @} */

/**
 * @brief   Configure Modbus ASCII transport
 */
static const modbus_ascii_params_t modbus_ascii_params[] =
{
    MODBUS_ASCII_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
