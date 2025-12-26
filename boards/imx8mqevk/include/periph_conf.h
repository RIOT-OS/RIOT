/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_imx8mqevk
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the NXP IMX8MQEVK board
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_CORECLOCK 266000000
#define XTIMER_HZ 1000000
/**
 * @name    Timer peripheral configuration
 * @{
 */
static const gpt_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = GPT1,
        .irq            = GPT1_IRQn,
        .clksrc         = 1,
        .prescaler      = 0,
        .flags          = 0,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    1
#define GPT_0_ISR           isr_gpt1
#define TIMER_NUMOF         (sizeof(timer_config)/sizeof(timer_config[0]))
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Debug COM Port */
        .dev      = UART2,
        .rx_pin   = GPIO_PIN(5, 24),
        .tx_pin   = GPIO_PIN(5, 25),
        .clk_src = 0
    },
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_uart2
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
