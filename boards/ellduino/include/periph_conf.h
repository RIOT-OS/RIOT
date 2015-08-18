/*
 * Copyright (C) 2014 ELL-i co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_ellduino
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Ellduino board
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (48000000U)         /* desired core clock frequency */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO		1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (47U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1
/** @} */


/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART1
#define UART_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_0_CLKDIS()     (RCC->APB2ENR &= (~RCC_APB2ENR_USART1EN))
#define UART_0_IRQ          USART1_IRQn
#define UART_0_ISR          isr_usart1
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_RX_PIN       15
#define UART_0_TX_PIN       14
#define UART_0_AF           0
/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
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
#define GPIO_0_PORT         GPIOA
#define GPIO_0_PIN          10
#define GPIO_0_CLK			17
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_0_IRQ          EXTI0_1_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          9
#define GPIO_1_CLK			17
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_1_IRQ          EXTI0_1_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOA
#define GPIO_2_PIN          8
#define GPIO_2_CLK			17
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_2_IRQ          EXTI0_1_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOC
#define GPIO_3_PIN          9
#define GPIO_3_CLK			19
#define GPIO_3_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_3_IRQ          EXTI0_1_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOC
#define GPIO_4_PIN          8
#define GPIO_4_CLK			19
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_4_IRQ          EXTI0_1_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOA
#define GPIO_5_PIN          7
#define GPIO_5_CLK			17
#define GPIO_5_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_5_IRQ          EXTI0_1_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOA
#define GPIO_6_PIN          3
#define GPIO_6_CLK			17
#define GPIO_6_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_6_IRQ          EXTI0_1_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOA
#define GPIO_7_PIN          2
#define GPIO_7_CLK			17
#define GPIO_7_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_7_IRQ          EXTI0_1_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOB
#define GPIO_8_PIN          10
#define GPIO_8_CLK			18
#define GPIO_8_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_8_IRQ          EXTI0_1_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOB
#define GPIO_9_PIN          11
#define GPIO_9_CLK			18
#define GPIO_9_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_9_IRQ          EXTI0_1_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOA
#define GPIO_10_PIN         15
#define GPIO_10_CLK			17
#define GPIO_10_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_10_IRQ          EXTI0_1_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOB
#define GPIO_11_PIN         5
#define GPIO_11_CLK			18
#define GPIO_11_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_11_IRQ          EXTI0_1_IRQn
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
