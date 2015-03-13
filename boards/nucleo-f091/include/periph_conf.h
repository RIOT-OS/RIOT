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

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

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
#define TIMER_0_PRESCALER   (47U)
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
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          (6U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          -1                      /* not configured */
#define GPIO_IRQ_1          -1                      /* not configured */
#define GPIO_IRQ_2          -1                      /* not configured */
#define GPIO_IRQ_3          -1                      /* not configured */
#define GPIO_IRQ_4          -1                      /* not configured */
#define GPIO_IRQ_5          -1                      /* not configured */
#define GPIO_IRQ_6          -1                      /* not configured */
#define GPIO_IRQ_7          -1                      /* not configured */
#define GPIO_IRQ_8          -1                      /* not configured */
#define GPIO_IRQ_9          -1                      /* not configured */
#define GPIO_IRQ_10         GPIO_0
#define GPIO_IRQ_11         GPIO_1
#define GPIO_IRQ_12         GPIO_2
#define GPIO_IRQ_13         GPIO_3
#define GPIO_IRQ_14         GPIO_4
#define GPIO_IRQ_15         GPIO_5

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOC
#define GPIO_0_PIN          10
#define GPIO_0_CLK          19
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC)
#define GPIO_0_IRQ          EXTI4_15_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOC
#define GPIO_1_PIN          11
#define GPIO_1_CLK          19
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC)
#define GPIO_1_IRQ          EXTI4_15_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOC
#define GPIO_2_PIN          12
#define GPIO_2_CLK          19
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC)
#define GPIO_2_IRQ          EXTI4_15_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOC
#define GPIO_3_PIN          13                   /* Used for user button 1 */
#define GPIO_3_CLK          19
#define GPIO_3_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC)
#define GPIO_3_IRQ          EXTI4_15_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOC
#define GPIO_4_PIN          14
#define GPIO_4_CLK          19
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PC)
#define GPIO_4_IRQ          EXTI4_15_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOC
#define GPIO_5_PIN          15
#define GPIO_5_CLK          19
#define GPIO_5_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PC)
#define GPIO_5_IRQ          EXTI4_15_IRQn
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
