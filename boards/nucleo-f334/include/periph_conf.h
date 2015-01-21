/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_nucleo-f334
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f334 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 **/
#define CLOCK_HSE           (8000000U)          /* external oscillator */
#define CLOCK_CORECLOCK     (72000000U)         /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_MUL       (CLOCK_CORECLOCK / CLOCK_HSE)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_1
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (71U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
/** @} */

/**
 * @brief UART configuration
 * @}
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLK          (CLOCK_CORECLOCK / 2)   /* UART clock runs with 32MHz (F_CPU / 1) */
#define UART_0_IRQ_CHAN     USART2_IRQn
#define UART_0_ISR          isr_usart2
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_RX_PIN       3
#define UART_0_TX_PIN       2
#define UART_0_AF           7
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
