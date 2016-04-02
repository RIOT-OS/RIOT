/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f091
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f091 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_HSE           (8000000U)          /* external oscillator */
#define CLOCK_CORECLOCK     (48000000U)         /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_MUL       (CLOCK_CORECLOCK / CLOCK_HSE)
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
#define TIMER_0_FREQ        (CLOCK_CORECLOCK)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_ISR         isr_tim2
/** @} */

/**
 * @brief UART configuration
 * @}
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLKDIS()     (RCC->APB1ENR &= (~RCC_APB1ENR_USART2EN))
#define UART_0_IRQ          USART2_IRQn
#define UART_0_ISR          isr_usart2
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_RX_PIN       3
#define UART_0_TX_PIN       2
#define UART_0_AF           1

/* UART 1 device configuration */
#define UART_1_DEV          USART1
#define UART_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_1_CLKDIS()     (RCC->APB2ENR &= (~RCC_APB2ENR_USART1EN))
#define UART_1_IRQ          USART1_IRQn
#define UART_1_ISR          isr_usart1
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOB
#define UART_1_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define UART_1_RX_PIN       7
#define UART_1_TX_PIN       6
#define UART_1_AF           0
/** @} */

/**
 * @name RTC configuration
 * @{
 */
/**
 * Nucleos with MB1136 C-02 or MB1136 C-03 -sticker on it have the required LSE
 * oscillator provided on the X2 slot.
 * See Nucleo User Manual UM1724 section 5.6.2.
 */
#define RTC_NUMOF           (1U)
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
#define ADC_NUMOF          (0)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
