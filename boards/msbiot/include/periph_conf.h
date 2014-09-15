/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_msbiot
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_HSE           (16000000U)         /* external oscillator */
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
#define UART_NUMOF          (3U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLKDIS()     (RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))
#define UART_0_CLK          (42000000)          /* UART clock runs with 42MHz (F_CPU / 4) */
#define UART_0_IRQ_CHAN     USART2_IRQn
#define UART_0_ISR          isr_usart2
/* UART 0 pin configuration */
#define UART_0_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define UART_0_PORT         GPIOA
#define UART_0_TX_PIN       2
#define UART_0_RX_PIN       3
#define UART_0_AF           7

/* UART 1 device configuration */
#define UART_1_DEV          USART1
#define UART_1_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_1_CLKDIS()     (RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN))
#define UART_1_CLK          (84000000)          /* UART clock runs with 84MHz (F_CPU / 2) */
#define UART_1_IRQ_CHAN     USART1_IRQn
#define UART_1_ISR          isr_usart1
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define UART_1_PORT         GPIOA
#define UART_1_TX_PIN       9
#define UART_1_RX_PIN       10
#define UART_1_AF           7

/* UART 2 device configuration */
#define UART_2_DEV          USART3
#define UART_2_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART3EN)
#define UART_2_CLKDIS()     (RCC->APB1ENR &= ~(RCC_APB1ENR_USART3EN))
#define UART_2_CLK          (42000000)          /* UART clock runs with 42MHz (F_CPU / 4) */
#define UART_2_IRQ_CHAN     USART3_IRQn
#define UART_2_ISR          isr_usart3
/* UART 2 pin configuration */
#define UART_2_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define UART_2_PORT         GPIOD
#define UART_2_TX_PIN       8
#define UART_2_RX_PIN       9
#define UART_2_AF           7
/** @} */


/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          16
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
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_0 /* alternatively GPIO_4 or GPIO_9 could be used here */
#define GPIO_IRQ_1          GPIO_1 /* alternatively GPIO_10 could be used here */
#define GPIO_IRQ_2          GPIO_15
#define GPIO_IRQ_3          -1/* not configured */
#define GPIO_IRQ_4          GPIO_2 /* alternatively GPIO_11 could be used here */
#define GPIO_IRQ_5          GPIO_12
#define GPIO_IRQ_6          -1/* not configured */
#define GPIO_IRQ_7          -1/* not configured */
#define GPIO_IRQ_8          GPIO_13
#define GPIO_IRQ_9          GPIO_5
#define GPIO_IRQ_10         GPIO_3
#define GPIO_IRQ_11         GPIO_6
#define GPIO_IRQ_12         GPIO_7
#define GPIO_IRQ_13         GPIO_8 /* alternatively GPIO_14 could be used here */
#define GPIO_IRQ_14         -1/* not configured */
#define GPIO_IRQ_15         -1/* not configured */

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA                   /* User Button 2 */
#define GPIO_0_PIN          0
#define GPIO_0_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_0_IRQ          EXTI0_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          1
#define GPIO_1_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA)
#define GPIO_1_IRQ          EXTI1_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOA
#define GPIO_2_PIN          4
#define GPIO_2_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA)
#define GPIO_2_IRQ          EXTI4_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOA                   /* CC3000 SPI_IRQ */
#define GPIO_3_PIN          10
#define GPIO_3_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define GPIO_3_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA)
#define GPIO_3_IRQ          EXTI15_10_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOB
#define GPIO_4_PIN          0
#define GPIO_4_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB)
#define GPIO_4_IRQ          EXTI0_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOB                   /* BEEPER Input */
#define GPIO_5_PIN          9
#define GPIO_5_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIO_5_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PB)
#define GPIO_5_IRQ          EXTI9_5_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOB                   /* IMU-9150 INT */
#define GPIO_6_PIN          11
#define GPIO_6_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIO_6_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PB)
#define GPIO_6_IRQ          EXTI15_10_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOB                   /* CC1101 CS */
#define GPIO_7_PIN          12
#define GPIO_7_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIO_7_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB)
#define GPIO_7_IRQ          EXTI15_10_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOB                   /* User Button 1 */
#define GPIO_8_PIN          13
#define GPIO_8_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define GPIO_8_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PB)
#define GPIO_8_IRQ          EXTI15_10_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOC                   /* TCA6416 Reset */
#define GPIO_9_PIN          0
#define GPIO_9_CLKEN()      (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_9_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC)
#define GPIO_9_IRQ          EXTI0_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOC                   /* CC3000 CS */
#define GPIO_10_PIN         1
#define GPIO_10_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_10_EXTI_CFG()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PC)
#define GPIO_10_IRQ         EXTI1_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOC                   /* CC1101 GDO 0 */
#define GPIO_11_PIN         4
#define GPIO_11_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_11_EXTI_CFG()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PC)
#define GPIO_11_IRQ         EXTI4_IRQn
/* GPIO channel 12 config */
#define GPIO_12_PORT        GPIOC                   /* CC1101 GDO 2 */
#define GPIO_12_PIN         5
#define GPIO_12_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_12_EXTI_CFG()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PC)
#define GPIO_12_IRQ         EXTI9_5_IRQn
/* GPIO channel 13 config */
#define GPIO_13_PORT        GPIOC                   /* TCA6416 INT */
#define GPIO_13_PIN         8
#define GPIO_13_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_13_EXTI_CFG()  (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PC)
#define GPIO_13_IRQ         EXTI9_5_IRQn
/* GPIO channel 14 config */
#define GPIO_14_PORT        GPIOC                   /* CC3000 VBAT_SW_EN */
#define GPIO_14_PIN         13
#define GPIO_14_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define GPIO_14_EXTI_CFG()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC)
#define GPIO_14_IRQ         EXTI15_10_IRQn
/* GPIO channel 15 config */
#define GPIO_15_PORT        GPIOD                   /* Micro SD Sockel CS */
#define GPIO_15_PIN         2
#define GPIO_15_CLKEN()     (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define GPIO_15_EXTI_CFG()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PD)
#define GPIO_15_IRQ         EXTI2_IRQn
/** @} */

#endif /* __PERIPH_CONF_H */
/** @} */
