/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32e230c-eval
 * @{
 *
 * @file
 * @brief       Default UART configuration for GD32E230 boards
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CFG_UART_DEFAULT_H
#define CFG_UART_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        //do we need all combinations of alternate functions for all pins?
        //how does it manage mutex combinations
        .dev        = USART0,
        .rcu_mask   = RCU_APB2EN_USART0EN_Msk,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        // .rx_af      = GPIO_AF_1,
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        // .tx_af      = GPIO_AF_2,
        // .de_pin     = ,
        // .de_af      = ,
        // .cts_pin    = ,
        // .cts_af     = ,
        // .rts_pin    = ,
        // .rts_af     = ,
        // .ck_pin     = ,
        // .ck_af      = ,
        .bus        = APB2,
        .irqn       = USART0_IRQn

    },
    {
        .dev        = USART1,
        .rcu_mask   = RCU_APB1EN_USART1EN_Msk,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        // .rx_af      = GPIO_AF_1,
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        // .tx_af      = GPIO_AF_1,
        // .de_pin     = ,
        // .de_af      = ,
        // .cts_pin    = ,
        // .cts_af     = ,
        // .rts_pin    = ,
        // .rts_af     = ,
        // .ck_pin     = ,
        // .ck_af      = ,
        .bus        = APB1,
        .irqn       = USART1_IRQn
    }
};

#define UART_0_ISR          (isr_usart0)
#define UART_1_ISR          (isr_usart1)

#define UART_0_IRQN         USART0_IRQn
#define UART_1_IRQN         USART1_IRQn

#define UART_NUMOF          ARRAY_SIZE(uart_config)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_UART_DEFAULT_H */
/** @} */
