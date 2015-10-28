/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_f4vi1
 * @{
 *
 * @file
 * @name       Peripheral MCU configuration for the F4VI1 board
 *
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
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
#define CLOCK_HSE           (16000000U)          /* external oscillator */
#define CLOCK_CORECLOCK     (168000000U)        /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_M         (CLOCK_HSE / 1000000)
#define CLOCK_PLL_N         ((CLOCK_CORECLOCK / 1000000) * 2)
#define CLOCK_PLL_P         (2U)
#define CLOCK_PLL_Q         (CLOCK_PLL_N / 48)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_5WS

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (83U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn

/* Timer 1 configuration */
#define TIMER_1_DEV         TIM5
#define TIMER_1_CHANNELS    4
#define TIMER_1_PRESCALER   (83U)
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM5EN)
#define TIMER_1_ISR         isr_tim5
#define TIMER_1_IRQ_CHAN    TIM5_IRQn
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    /* device, RCC mask, RX pin, TX pin, pin AF, IRQ channel, DMA stream, DMA  */
    {
        USART6,                     /* device base register */
        RCC_APB2ENR_USART6EN,       /* RCC mask */
        GPIO_PIN(PORT_C,7),         /* RX pin */
        GPIO_PIN(PORT_C,6),         /* TX pin */
        GPIO_AF8,                   /* pin AF */
        USART6_IRQn,                /* IRQ channel */
        14,                         /* DMA stream */
        5                           /* DMA channel */
    },
};

/* assign ISR vector names */
#define UART_0_ISR          isr_usart6
#define UART_0_DMA_ISR      isr_dma2_stream6

/* deduct number of defined UART interfaces */
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
