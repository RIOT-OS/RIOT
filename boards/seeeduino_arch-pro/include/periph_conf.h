/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_seeduino_arch-pro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Seeeduino Archo Pro board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
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
        .dev = (LPC_UART_TypeDef*)LPC_UART3,
        .irq_rx = UART3_IRQn,
        .clk_offset = 25,
        .pinsel = 0,
        .pinsel_shift = 0,
        .pinsel_af = 2
    }
};

#define UART_IRQ_PRIO       1

#define UART_0_ISR          isr_uart0
#define UART_1_ISR          isr_uart3

#define UART_NUMOF          ARRAY_SIZE(uart_config)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
