/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F3discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

/**
 * @name Clock system configuration
 * @{
 */
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
 * @{
 */
#define UART_NUMOF          (3U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART1
#define UART_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_0_CLK          (CLOCK_CORECLOCK / 1)   /* UART clock runs with 72MHz (F_CPU / 1) */
#define UART_0_IRQ_CHAN     USART1_IRQn
#define UART_0_ISR          isr_usart1
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_PORT         GPIOA
#define UART_0_TX_PIN       9
#define UART_0_RX_PIN       10
#define UART_0_AF           7

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_1_CLK          (CLOCK_CORECLOCK / 2)   /* UART clock runs with 36MHz (F_CPU / 2) */
#define UART_1_IRQ_CHAN     USART2_IRQn
#define UART_1_ISR          isr_usart2
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIODEN)
#define UART_1_PORT         GPIOD
#define UART_1_TX_PIN       5
#define UART_1_RX_PIN       6
#define UART_1_AF           7

/* UART 1 device configuration */
#define UART_2_DEV          USART3
#define UART_2_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART3EN)
#define UART_2_CLK          (CLOCK_CORECLOCK / 2)  /* UART clock runs with 36MHz (F_CPU / 2) */
#define UART_2_IRQ_CHAN     USART3_IRQn
#define UART_2_ISR          isr_usart3
/* UART 1 pin configuration */
#define UART_2_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIODEN)
#define UART_2_PORT         GPIOD
#define UART_2_TX_PIN       8
#define UART_2_RX_PIN       9
#define UART_2_AF           7
/** @} */

/**
 * @brief GPIO configuration
 * @{
 */
#define GPIO_NUMOF          12
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
#define GPIO_IRQ_0          GPIO_11 /* alternatively GPIO_4 could be used here */
#define GPIO_IRQ_1          GPIO_5
#define GPIO_IRQ_2          GPIO_0
#define GPIO_IRQ_3          GPIO_3
#define GPIO_IRQ_4          GPIO_1
#define GPIO_IRQ_5          GPIO_2
#define GPIO_IRQ_6          (-1)    /* not configured */
#define GPIO_IRQ_7          (-1)    /* not configured */
#define GPIO_IRQ_8          (-1)    /* not configured */
#define GPIO_IRQ_9          (-1)    /* not configured */
#define GPIO_IRQ_10         (-1)    /* not configured */
#define GPIO_IRQ_11         GPIO_6
#define GPIO_IRQ_12         GPIO_7
#define GPIO_IRQ_13         GPIO_8
#define GPIO_IRQ_14         GPIO_9
#define GPIO_IRQ_15         GPIO_10

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOE                   /* LSM303DLHC -> DRDY */
#define GPIO_0_PIN          2
#define GPIO_0_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_0_EXTI_CFG1()  (SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI2))
#define GPIO_0_EXTI_CFG2()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PE)
#define GPIO_0_IRQ          EXTI2_TSC_IRQn

/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOE                   /* LSM303DLHC -> INT1 */
#define GPIO_1_PIN          4
#define GPIO_1_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_1_EXTI_CFG1()  (SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI4))
#define GPIO_1_EXTI_CFG2()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PE)
#define GPIO_1_IRQ          EXTI4_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOE                   /* LSM303DLHC -> INT2 */
#define GPIO_2_PIN          5
#define GPIO_2_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_2_EXTI_CFG1()  (SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5))
#define GPIO_2_EXTI_CFG2()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PE)
#define GPIO_2_IRQ          EXTI9_5_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOE                   /* L2GD20 -> CS */
#define GPIO_3_PIN          3
#define GPIO_3_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_3_EXTI_CFG1()  (SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI3))
#define GPIO_3_EXTI_CFG2()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PE)
#define GPIO_3_IRQ          EXTI3_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOE                   /* L2GD20 -> INT1 */
#define GPIO_4_PIN          0
#define GPIO_4_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_4_EXTI_CFG1()  (SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0))
#define GPIO_4_EXTI_CFG2()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PE)
#define GPIO_4_IRQ          EXTI0_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOE                   /* L2GD20 -> INT2/DRDY */
#define GPIO_5_PIN          1
#define GPIO_5_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOEEN)
#define GPIO_5_EXTI_CFG1()  (SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1))
#define GPIO_5_EXTI_CFG2()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PE)
#define GPIO_5_IRQ          EXTI1_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOB
#define GPIO_6_PIN          11
#define GPIO_6_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_6_EXTI_CFG1()  (SYSCFG->EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI11))
#define GPIO_6_EXTI_CFG2()  (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB)
#define GPIO_6_IRQ          EXTI15_10_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOB
#define GPIO_7_PIN          12
#define GPIO_7_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_7_EXTI_CFG1()  (SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI12))
#define GPIO_7_EXTI_CFG2()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB)
#define GPIO_7_IRQ          EXTI15_10_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOB
#define GPIO_8_PIN          13
#define GPIO_8_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_8_EXTI_CFG1()  (SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13))
#define GPIO_8_EXTI_CFG2()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB)
#define GPIO_8_IRQ          EXTI15_10_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOB
#define GPIO_9_PIN          14
#define GPIO_9_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_9_EXTI_CFG1()  (SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI14))
#define GPIO_9_EXTI_CFG2()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PB)
#define GPIO_9_IRQ          EXTI15_10_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOB
#define GPIO_10_PIN         15
#define GPIO_10_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_10_EXTI_CFG1() (SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI15))
#define GPIO_10_EXTI_CFG2() (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PB)
#define GPIO_10_IRQ         EXTI15_10_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOA                   /* User button 1 */
#define GPIO_11_PIN         0
#define GPIO_11_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_11_EXTI_CFG1() (SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0))
#define GPIO_11_EXTI_CFG2() (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_11_IRQ          EXTI0_IRQn
/** @} */

#endif /* __PERIPH_CONF_H */
