/*
 * Copyright (C) 2021 Luo Jia (HUST IoT Security Lab)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_alientek-pandora
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Alientek Pandora board
 *
 * @author      Luo Jia <luojia@hust.edu.cn>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1   /**< This board provides LSE  */
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
 * @{
 */
/** All timers on board */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5 /**< Timer 0 ISR number */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Number of timers on this board */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
/** All UARTs on board */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart1) /**< Usart1 ISR number */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< Number of uarts on this board */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
