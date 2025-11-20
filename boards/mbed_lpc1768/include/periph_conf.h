/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mbed_lpc1768
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the mbed LPC1768 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph_cpu.h"
#include "vendor/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         LPC_TIM0
#define TIMER_0_CHANNELS    4
#define TIMER_0_FREQ        (96000000ul)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (LPC_SC->PCONP |= (1 << 1))
#define TIMER_0_CLKDIS()    (LPC_SC->PCONP &= ~(1 << 1))
#define TIMER_0_PLKSEL()    (LPC_SC->PCLKSEL0 |= (1 << 2))
#define TIMER_0_ISR         isr_timer0
#define TIMER_0_IRQ         TIMER0_IRQn
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = (LPC_UART_TypeDef*)LPC_UART0,
        .irq_rx = UART0_IRQn,
        .clk_offset = 3,
        .pinsel = 0,
        .pinsel_shift = 2,
        .pinsel_af = 1,
    },
    {
        .dev = (LPC_UART_TypeDef*)LPC_UART2,
        .irq_rx = UART2_IRQn,
        .clk_offset = 24,
        .pinsel = 0,
        .pinsel_shift = 10,
        .pinsel_af = 1,
    }
};

#define UART_IRQ_PRIO       1

#define UART_0_ISR          isr_uart0
#define UART_1_ISR          isr_uart2

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
