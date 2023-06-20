/*
 * Copyright (C) 2023-2024 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_modbus
 *
 * @{
 * @file
 * @brief       Default configuration for Modbus RTU
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef MODBUS_RTU_PARAMS_H
#define MODBUS_RTU_PARAMS_H

#include "board.h"
#include "modbus_rtu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Modbus RTU
 * @{
 */
#ifdef MODULE_MODBUS_RTU
#ifndef MODBUS_RTU_PARAM_UART_DEV
#define MODBUS_RTU_PARAM_UART_DEV        UART_DEV(1)
#endif

#ifndef MODBUS_RTU_PARAM_UART_BAUDRATE
#define MODBUS_RTU_PARAM_UART_BAUDRATE   9600
#endif

#ifndef MODBUS_RTU_PARAM_PIN_RTS
#define MODBUS_RTU_PARAM_PIN_RTS         GPIO_UNDEF
#endif

#ifndef MODBUS_RTU_PARAM_PIN_RTS_ENABLE
#define MODBUS_RTU_PARAM_PIN_RTS_ENABLE  1
#endif

#ifndef MODBUS_RTU_PARAMS
#define MODBUS_RTU_PARAMS   { \
            .uart = MODBUS_RTU_PARAM_UART_DEV, \
            .baudrate = MODBUS_RTU_PARAM_UART_BAUDRATE, \
            .pin_rts = MODBUS_RTU_PARAM_PIN_RTS, \
            .pin_rts_enable = MODBUS_RTU_PARAM_PIN_RTS_ENABLE \
}
#endif
#endif /* MODULE_MODBUS_RTU */

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

#endif /* MODBUS_RTU_PARAMS_H */
/** @} */
