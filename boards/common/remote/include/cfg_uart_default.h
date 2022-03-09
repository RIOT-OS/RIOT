/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Common default UART configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef CFG_UART_DEFAULT_H
#define CFG_UART_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    /* UART0 is mapped to debug usb */
    {
        .dev      = UART0_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_A, 0),
        .tx_pin   = GPIO_PIN(PORT_A, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    },
    {
        .dev      = UART1_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_C, 1),
        .tx_pin   = GPIO_PIN(PORT_C, 0),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_uart0
#define UART_1_ISR          isr_uart1

/* macros common across all UARTs */
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CFG_UART_DEFAULT_H */
/** @} */
