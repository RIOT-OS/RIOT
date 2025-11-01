/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_soft_uart
 * @{
 *
 * @file
 * @brief       Software UART configuration
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "soft_uart.h"
#include "macros/units.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOFT_UART_PARAM_RX
#define SOFT_UART_PARAM_RX          GPIO_UNDEF
#endif
#ifndef SOFT_UART_PARAM_TX
#define SOFT_UART_PARAM_TX          GPIO_UNDEF
#endif
#ifndef SOFT_UART_PARAM_TIMER_RX
#define SOFT_UART_PARAM_TIMER_RX    (0)
#endif
#ifndef SOFT_UART_PARAM_TIMER_TX
#define SOFT_UART_PARAM_TIMER_TX    (1)
#endif
#ifndef SOFT_UART_PARAM_FREQ
#define SOFT_UART_PARAM_FREQ        MHZ(1)
#endif
#ifndef SOFT_UART_PARAM_FLAGS
#define SOFT_UART_PARAM_FLAGS       (0)
#endif

#ifndef SOFT_UART_PARAMS
#define SOFT_UART_PARAMS     { .rx_pin = SOFT_UART_PARAM_RX,    \
                               .tx_pin = SOFT_UART_PARAM_TX,    \
                               .rx_timer = SOFT_UART_PARAM_TIMER_RX, \
                               .tx_timer = SOFT_UART_PARAM_TIMER_TX, \
                               .timer_freq = SOFT_UART_PARAM_FREQ, \
                               .flags = SOFT_UART_PARAM_FLAGS, \
                             }
#endif

/**
 * @brief   Sotware UART port descriptor array
 */
static const soft_uart_conf_t soft_uart_config[] = {
    SOFT_UART_PARAMS,
};

#define SOFT_UART_NUMOF             ARRAY_SIZE(soft_uart_config)

#ifdef __cplusplus
}
#endif

/** @} */
