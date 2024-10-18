/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_soft_uart
 * @{
 *
 * @file
 * @brief       Software UART configuration
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef SOFT_UART_PARAMS_H
#define SOFT_UART_PARAMS_H

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
#endif /* SOFT_UART_PARAMS_H */
