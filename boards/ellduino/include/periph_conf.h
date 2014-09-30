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

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

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
#define TIMER_NUMOF         (6U)
#define TIMER_0_EN          1
#define TIMER_1_EN          0
#define TIMER_2_EN          0
#define TIMER_3_EN          0
#define TIMER_4_EN          0
#define TIMER_5_EN          0

/* Timer 0 configuration */
#define TIMER_0_DEV         TIM2
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (47U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         isr_tim2
#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1

/* Timer 1 configuration */
#define TIMER_1_DEV         TIM3
#define TIMER_1_CHANNELS    4
#define TIMER_1_PRESCALER   (47U)
#define TIMER_1_MAX_VALUE   (0x0000ffff)
#define TIMER_1_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
#define TIMER_1_ISR         isr_tim3
#define TIMER_1_IRQ_CHAN    TIM3_IRQn
#define TIMER_1_IRQ_PRIO    1

/* Timer 2 configuration */
#define TIMER_2_DEV         TIM14
#define TIMER_2_CHANNELS    1
#define TIMER_2_PRESCALER   (47U)
#define TIMER_2_MAX_VALUE   (0x0000ffff)
#define TIMER_2_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM14EN)
#define TIMER_2_ISR         isr_tim14
#define TIMER_2_IRQ_CHAN    TIM14_IRQn
#define TIMER_2_IRQ_PRIO    1

/* Timer 3 configuration */
#define TIMER_3_DEV         TIM15
#define TIMER_3_CHANNELS    2
#define TIMER_3_PRESCALER   (47U)
#define TIMER_3_MAX_VALUE   (0x0000ffff)
#define TIMER_3_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM15EN)
#define TIMER_3_ISR         isr_tim15
#define TIMER_3_IRQ_CHAN    TIM15_IRQn
#define TIMER_3_IRQ_PRIO    1

/* Timer 4 configuration */
#define TIMER_4_DEV         TIM16
#define TIMER_4_CHANNELS    1
#define TIMER_4_PRESCALER   (47U)
#define TIMER_4_MAX_VALUE   (0x0000ffff)
#define TIMER_4_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM16EN)
#define TIMER_4_ISR         isr_tim16
#define TIMER_4_IRQ_CHAN    TIM16_IRQn
#define TIMER_4_IRQ_PRIO    1

/* Timer 5 configuration */
#define TIMER_5_DEV         TIM17
#define TIMER_5_CHANNELS    1
#define TIMER_5_PRESCALER   (47U)
#define TIMER_5_MAX_VALUE   (0x0000ffff)
#define TIMER_5_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM17EN)
#define TIMER_5_ISR         isr_tim17
#define TIMER_5_IRQ_CHAN    TIM17_IRQn
#define TIMER_5_IRQ_PRIO    1
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           0
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

/* UART 1 device configuration */
#define UART_1_DEV          USART2
#define UART_1_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_1_CLKDIS()     (RCC->APB1ENR &= (~RCC_APB1ENR_USART2EN))
#define UART_1_IRQ          USART2_IRQn
#define UART_1_ISR          isr_usart2
/* UART 1 pin configuration */
#define UART_1_PORT         GPIOA
#define UART_1_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_1_RX_PIN       10
#define UART_1_TX_PIN       9
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
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          22
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
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_18_EN          1
#define GPIO_19_EN          1
#define GPIO_20_EN          1
#define GPIO_21_EN          1

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA
#define GPIO_0_PIN          10
#define GPIO_0_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOA
#define GPIO_1_PIN          9
#define GPIO_1_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOA
#define GPIO_2_PIN          8
#define GPIO_2_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOC
#define GPIO_3_PIN          9
#define GPIO_3_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOC
#define GPIO_4_PIN          8
#define GPIO_4_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOA
#define GPIO_5_PIN          7
#define GPIO_5_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOA
#define GPIO_6_PIN          3
#define GPIO_6_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOA
#define GPIO_7_PIN          2
#define GPIO_7_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOB
#define GPIO_8_PIN          10
#define GPIO_8_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOB
#define GPIO_9_PIN          11
#define GPIO_9_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOA
#define GPIO_10_PIN         15
#define GPIO_10_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOB
#define GPIO_11_PIN         5
#define GPIO_11_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 12 config */
#define GPIO_12_PORT        GPIOB
#define GPIO_12_PIN         4
#define GPIO_12_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 13 config */
#define GPIO_13_PORT        GPIOB
#define GPIO_13_PIN         3
#define GPIO_13_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 14 config */
#define GPIO_14_PORT        GPIOC
#define GPIO_14_PIN         6
#define GPIO_14_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
/* GPIO channel 15 config */
#define GPIO_15_PORT        GPIOC
#define GPIO_15_PIN         7
#define GPIO_15_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
/* GPIO channel 16 config */
#define GPIO_16_PORT        GPIOA
#define GPIO_16_PIN         14
#define GPIO_16_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
/* GPIO channel 17 config */
#define GPIO_17_PORT        GPIOD
#define GPIO_17_PIN         2
#define GPIO_17_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIODEN)
/* GPIO channel 18 config */
#define GPIO_18_PORT        GPIOB
#define GPIO_18_PIN         6
#define GPIO_18_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 19 config */
#define GPIO_19_PORT        GPIOB
#define GPIO_19_PIN         7
#define GPIO_19_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 20 config */
#define GPIO_20_PORT        GPIOB
#define GPIO_20_PIN         9
#define GPIO_20_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/* GPIO channel 21 config */
#define GPIO_21_PORT        GPIOB
#define GPIO_21_PIN         8
#define GPIO_21_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
/** @} */

#endif /* __PERIPH_CONF_H */
