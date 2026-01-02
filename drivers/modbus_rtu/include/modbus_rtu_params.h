/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_modbus
 *
 * @{
 * @file
 * @brief       Default configuration for Modbus RTU
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
 * @name    Set default configuration parameters for the Modbus RTU
 * @{
 */
#ifdef MODULE_MODBUS_RTU
#  ifndef MODBUS_RTU_PARAM_PIN_RTS
#    define MODBUS_RTU_PARAM_PIN_RTS            GPIO_UNDEF
#  endif

#  ifndef MODBUS_RTU_PARAM_PIN_RTS_ENABLE
#    define MODBUS_RTU_PARAM_PIN_RTS_ENABLE     1
#  endif

#  ifndef MODBUS_RTU_PARAM_UART_DEV
#    define MODBUS_RTU_PARAM_UART_DEV           UART_DEV(1)
#  endif

#  ifndef MODBUS_RTU_PARAM_UART_BAUDRATE
#    define MODBUS_RTU_PARAM_UART_BAUDRATE      9600
#  endif

#  ifndef MODBUS_RTU_PARAM_RESPONSE_TIMEOUT
#    define MODBUS_RTU_PARAM_RESPONSE_TIMEOUT   1000000
#  endif

#  ifndef MODBUS_RTU_PARAMS
#    define MODBUS_RTU_PARAMS   { .pin_rts = MODBUS_RTU_PARAM_PIN_RTS, \
                                  .pin_rts_enable = MODBUS_RTU_PARAM_PIN_RTS_ENABLE, \
                                  .uart = MODBUS_RTU_PARAM_UART_DEV, \
                                  .baudrate = MODBUS_RTU_PARAM_UART_BAUDRATE, \
                                  .response_timeout = MODBUS_RTU_PARAM_RESPONSE_TIMEOUT }
#  endif
#endif
/** @}*/

/**
 * @brief   Configure Modbus RTU
 */
static const modbus_rtu_params_t modbus_rtu_params[] =
{
    MODBUS_RTU_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
