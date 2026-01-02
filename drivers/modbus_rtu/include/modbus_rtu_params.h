/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_modbus_rtu
 *
 * @{
 * @file
 * @brief       Default configuration for Modbus RTU transport
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"
#include "time_units.h"

#include "modbus_rtu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Modbus RTU transport
 * @{
 */
#ifndef MODBUS_RTU_PARAM_PIN_RTS
#  define MODBUS_RTU_PARAM_PIN_RTS              GPIO_UNDEF
#endif

#ifndef MODBUS_RTU_PARAM_PIN_RTS_ENABLE
#  define MODBUS_RTU_PARAM_PIN_RTS_ENABLE       (true)
#endif

#ifndef MODBUS_RTU_PARAM_UART_DEV
#  define MODBUS_RTU_PARAM_UART_DEV             UART_DEV(1)
#endif

#ifndef MODBUS_RTU_PARAM_UART_BAUDRATE
#  define MODBUS_RTU_PARAM_UART_BAUDRATE        (9600)
#endif

#ifndef MODBUS_RTU_PARAM_RESPONSE_TIMEOUT
#  define MODBUS_RTU_PARAM_RESPONSE_TIMEOUT     (1U * US_PER_SEC)
#endif

#ifndef MODBUS_RTU_PARAMS
#  define MODBUS_RTU_PARAMS   { .pin_rts = MODBUS_RTU_PARAM_PIN_RTS, \
                                .pin_rts_enable = MODBUS_RTU_PARAM_PIN_RTS_ENABLE, \
                                .uart = MODBUS_RTU_PARAM_UART_DEV, \
                                .baudrate = MODBUS_RTU_PARAM_UART_BAUDRATE, \
                                .response_timeout = MODBUS_RTU_PARAM_RESPONSE_TIMEOUT }
#endif
/** @} */

/**
 * @brief   Configure Modbus RTU transport
 */
static const modbus_rtu_params_t modbus_rtu_params[] =
{
    MODBUS_RTU_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
