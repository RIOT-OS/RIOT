/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32f0discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F0discovery board
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
 * @name Timer configuration
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
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART1
#define UART_0_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
#define UART_0_CLKDIS()     (RCC->APB2ENR &= (~RCC_APB2ENR_USART1EN))
#define UART_0_IRQ          USART1_IRQn
#define UART_0_ISR          isr_usart1
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOB
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define UART_0_RX_PIN       7
#define UART_0_TX_PIN       6
#define UART_0_AF           0

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_1_CLKDIS()     (RCC->APB1ENR &= (~RCC_APB1ENR_USART2EN))
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_1_RX_PIN       3
#define UART_1_TX_PIN       2
#define UART_1_AF           1
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (1U)
#define ADC_0_EN            1
#define ADC_MAX_CHANNELS    6

/* ADC 0 configuration */
#define ADC_0_DEV           ADC1
#define ADC_0_CHANNELS      6
#define ADC_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADCEN)
#define ADC_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADCEN))
#define ADC_0_PORT          GPIOC
#define ADC_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0           10
#define ADC_0_CH0_PIN       0
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           11
#define ADC_0_CH1_PIN       1
/* ADC 0 channel 2 pin config */
#define ADC_0_CH2           12
#define ADC_0_CH2_PIN       2
/* ADC 0 channel 3 pin config */
#define ADC_0_CH3           13
#define ADC_0_CH3_PIN       3
/* ADC 0 channel 4 pin config */
#define ADC_0_CH4           14
#define ADC_0_CH4_PIN       4
/* ADC 0 channel 5 pin config */
#define ADC_0_CH5           15
#define ADC_0_CH5_PIN       5
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_IRQ_PRIO        1

/* SPI 0 device config */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_ISR           isr_spi1
/* SPI 1 pin configuration */
#define SPI_0_PORT          GPIOA
#define SPI_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define SPI_0_PIN_SCK       5
#define SPI_0_PIN_MISO      6
#define SPI_0_PIN_MOSI      7
#define SPI_0_PIN_AF        0

/* SPI 1 device config */
#define SPI_1_DEV           SPI2
#define SPI_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_SPI2EN))
#define SPI_1_IRQ           SPI2_IRQn
#define SPI_1_ISR           isr_spi2
/* SPI 1 pin configuration */
#define SPI_1_PORT          GPIOB
#define SPI_1_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define SPI_1_PIN_SCK       13
#define SPI_1_PIN_MISO      14
#define SPI_1_PIN_MOSI      15
#define SPI_1_PIN_AF        0
/** @} */

/**
 * @name GPIO configuration
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
#define GPIO_IRQ_0          GPIO_0
#define GPIO_IRQ_1          GPIO_1
#define GPIO_IRQ_2          -1                      /* not configured */
#define GPIO_IRQ_3          -1                      /* not configured */
#define GPIO_IRQ_4          GPIO_2
#define GPIO_IRQ_5          GPIO_3
#define GPIO_IRQ_6          GPIO_4
#define GPIO_IRQ_7          GPIO_5
#define GPIO_IRQ_8          -1                      /* not configured */
#define GPIO_IRQ_9          -1                      /* not configured */
#define GPIO_IRQ_10         GPIO_6
#define GPIO_IRQ_11         GPIO_7
#define GPIO_IRQ_12         GPIO_8
#define GPIO_IRQ_13         GPIO_9
#define GPIO_IRQ_14         GPIO_10
#define GPIO_IRQ_15         GPIO_11

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA                   /* Used for user button 1 */
#define GPIO_0_PIN          0
#define GPIO_0_CLK          17                      /* 0: PORT A, 1: B ... */
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_0_IRQ          EXTI0_1_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          1
#define GPIO_1_CLK          17
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA)
#define GPIO_1_IRQ          EXTI0_1_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOF
#define GPIO_2_PIN          4
#define GPIO_2_CLK          22
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PF)
#define GPIO_2_IRQ          EXTI4_15_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOF
#define GPIO_3_PIN          5
#define GPIO_3_CLK          22
#define GPIO_3_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PF)
#define GPIO_3_IRQ          EXTI4_15_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOF
#define GPIO_4_PIN          6
#define GPIO_4_CLK          22
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PF)
#define GPIO_4_IRQ          EXTI4_15_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOF
#define GPIO_5_PIN          7
#define GPIO_5_CLK          22
#define GPIO_5_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PF)
#define GPIO_5_IRQ          EXTI4_15_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOC
#define GPIO_6_PIN          10
#define GPIO_6_CLK          19
#define GPIO_6_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC)
#define GPIO_6_IRQ          EXTI4_15_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOC
#define GPIO_7_PIN          11
#define GPIO_7_CLK          19
#define GPIO_7_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC)
#define GPIO_7_IRQ          EXTI4_15_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOC
#define GPIO_8_PIN          12
#define GPIO_8_CLK          19
#define GPIO_8_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC)
#define GPIO_8_IRQ          EXTI4_15_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOC
#define GPIO_9_PIN          13
#define GPIO_9_CLK          19
#define GPIO_9_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC)
#define GPIO_9_IRQ          EXTI4_15_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOC
#define GPIO_10_PIN         14
#define GPIO_10_CLK         19
#define GPIO_10_EXTI_CFG()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PC)
#define GPIO_10_IRQ         EXTI4_15_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOC
#define GPIO_11_PIN         15
#define GPIO_11_CLK         19
#define GPIO_11_EXTI_CFG()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PC)
#define GPIO_11_IRQ         EXTI4_15_IRQn
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
