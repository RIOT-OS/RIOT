/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Adam Berlinger
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nucleo-f401
 * @{
 *
 * @file
 * @name       Peripheral MCU configuration for the Nucleo F401RE evaluation board
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Adam Berlinger <berlingeradam@gmail.com>
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
#define CLOCK_USE_HSI       (1)                /* use internal oscillator */
#define CLOCK_HSI           (16000000U)        /* internal oscillator frequency */
#define CLOCK_CORECLOCK     (84000000U)        /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_M         (CLOCK_HSI / 1000000)
#define CLOCK_PLL_N         ((CLOCK_CORECLOCK / 1000000) * 4)
#define CLOCK_PLL_P         (4U)
#define CLOCK_PLL_Q         (CLOCK_PLL_N / 48)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
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
#define TIMER_0_DEV         TIM9
#define TIMER_0_CHANNELS    2
#define TIMER_0_PRESCALER   (84U)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (RCC->APB2ENR |= RCC_APB2ENR_TIM9EN)
#define TIMER_0_ISR         isr_tim1_brk_tim9
#define TIMER_0_IRQ_CHAN    TIM1_BRK_TIM9_IRQn

/* Timer 1 configuration */
#define TIMER_1_DEV         TIM5
#define TIMER_1_CHANNELS    4
#define TIMER_1_PRESCALER   (42U)
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
#define UART_IRQ_PRIO       1
#define UART_CLK            (14000000U)         /* UART clock runs with 14MHz */

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLKDIS()     (RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))
#define UART_0_CLK          (21000000)          /* UART clock runs with 42MHz (F_CPU / 4) */
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
#define UART_1_CLK          (42000000)          /* UART clock runs with 42MHz (F_CPU / 4) */
#define UART_1_IRQ_CHAN     USART1_IRQn
#define UART_1_ISR          isr_usart1
/* UART 1 pin configuration */
#define UART_1_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define UART_1_PORT         GPIOA
#define UART_1_TX_PIN       9
#define UART_1_RX_PIN       10
#define UART_1_AF           7

/* UART 2 device configuration */
#define UART_2_DEV          USART6
#define UART_2_CLKEN()      (RCC->APB2ENR |= RCC_APB2ENR_USART6EN)
#define UART_2_CLKDIS()     (RCC->APB2ENR &= ~(RCC_APB2ENR_USART6EN))
#define UART_2_CLK          (42000000)          /* UART clock runs with 42MHz (F_CPU / 4) */
#define UART_2_IRQ_CHAN     USART6_IRQn
#define UART_2_ISR          isr_usart6
/* UART 1 pin configuration */
#define UART_2_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define UART_2_PORT         GPIOA
#define UART_2_TX_PIN       11
#define UART_2_RX_PIN       12
#define UART_2_AF           8
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (1U)
#define ADC_0_EN            1
#define ADC_MAX_CHANNELS    2

/* ADC 0 configuration */
#define ADC_0_DEV           ADC1
#define ADC_0_CHANNELS      2
#define ADC_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_ADC1EN)
#define ADC_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_ADC1EN))
#define ADC_0_PORT          GPIOA
#define ADC_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
/* ADC 0 channel 0 pin config */
#define ADC_0_CH0           1
#define ADC_0_CH0_PIN       1
/* ADC 0 channel 1 pin config */
#define ADC_0_CH1           4
#define ADC_0_CH1_PIN       4
/** @} */

/**
 * @name DAC configuration
 * @{
 */
#define DAC_NUMOF          (0U)

/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (2U)
#define PWM_0_EN            1
#define PWM_1_EN            1
#define PWM_MAX_CHANNELS    4

/* PWM 0 device configuration */
#define PWM_0_DEV           TIM4
#define PWM_0_CHANNELS      1
#define PWM_0_CLK           (42000000U)
#define PWM_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
#define PWM_0_CLKDIS()      (RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN)
/* PWM 0 pin configuration */
#define PWM_0_PORT          GPIOB
#define PWM_0_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define PWM_0_PIN_CH0       6
#define PWM_0_PIN_AF        2

/* PWM 1 device configuration */
#define PWM_1_DEV           TIM3
#define PWM_1_CHANNELS      2
#define PWM_1_CLK           (42000000U)
#define PWM_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
#define PWM_1_CLKDIS()      (RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN)
/* PWM 1 pin configuration */
#define PWM_1_PORT          GPIOB
#define PWM_1_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define PWM_1_PIN_CH0       5
#define PWM_1_PIN_CH1       5
#define PWM_1_PIN_AF        2

/* PWM 2 device configuration */
#define PWM_2_DEV           TIM2
#define PWM_2_CHANNELS      3
#define PWM_2_CLK           (84000000U)
#define PWM_2_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define PWM_2_CLKDIS()      (RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN)
/* PWM 2 pin configuration */
#define PWM_2_PORT          GPIOB
#define PWM_2_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define PWM_2_PIN_CH0       10
#define PWM_2_PIN_CH1       10
#define PWM_2_PIN_CH2       10
#define PWM_2_PIN_AF        1
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (0U)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (3U)
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_2_EN            1
#define SPI_IRQ_PRIO        1

/* SPI 0 device config */
#define SPI_0_DEV               SPI1
#define SPI_0_CLKEN()           (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()          (RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN)
#define SPI_0_BUS_DIV           1   /* 1 -> SPI runs with half CPU clock, 0 -> quarter CPU clock */
#define SPI_0_IRQ               SPI1_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          GPIOA
#define SPI_0_SCK_PIN           5
#define SPI_0_SCK_AF            5
#define SPI_0_SCK_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define SPI_0_MISO_PORT         GPIOA
#define SPI_0_MISO_PIN          6
#define SPI_0_MISO_AF           5
#define SPI_0_MISO_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define SPI_0_MOSI_PORT         GPIOA
#define SPI_0_MOSI_PIN          7
#define SPI_0_MOSI_AF           5
#define SPI_0_MOSI_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)

/* SPI 1 device config */
#define SPI_1_DEV               SPI2
#define SPI_1_CLKEN()           (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_1_CLKDIS()          (RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN)
#define SPI_1_BUS_DIV           0   /* 1 -> SPI runs with half CPU clock, 0 -> quarter CPU clock */
#define SPI_1_IRQ               SPI2_IRQn
#define SPI_1_IRQ_HANDLER       isr_spi2
/* SPI 1 pin configuration */
#define SPI_1_SCK_PORT          GPIOB
#define SPI_1_SCK_PIN           13
#define SPI_1_SCK_AF            5
#define SPI_1_SCK_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define SPI_1_MISO_PORT         GPIOB
#define SPI_1_MISO_PIN          14
#define SPI_1_MISO_AF           5
#define SPI_1_MISO_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define SPI_1_MOSI_PORT         GPIOB
#define SPI_1_MOSI_PIN          15
#define SPI_1_MOSI_AF           5
#define SPI_1_MOSI_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)

/* SPI 2 device config */
#define SPI_2_DEV               SPI3
#define SPI_2_CLKEN()           (RCC->APB1ENR |= RCC_APB1ENR_SPI3EN)
#define SPI_2_CLKDIS()          (RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN)
#define SPI_2_BUS_DIV           0   /* 1 -> SPI runs with half CPU clock, 0 -> quarter CPU clock */
#define SPI_2_IRQ               SPI3_IRQn
#define SPI_2_IRQ_HANDLER       isr_spi3
/* SPI 2 pin configuration */
#define SPI_2_SCK_PORT          GPIOC
#define SPI_2_SCK_PIN           10
#define SPI_2_SCK_AF            5
#define SPI_2_SCK_PORT_CLKEN()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define SPI_2_MISO_PORT         GPIOC
#define SPI_2_MISO_PIN          11
#define SPI_2_MISO_AF           5
#define SPI_2_MISO_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define SPI_2_MOSI_PORT         GPIOC
#define SPI_2_MOSI_PIN          12
#define SPI_2_MOSI_AF           5
#define SPI_2_MOSI_PORT_CLKEN() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (2U)
#define I2C_0_EN            1
#define I2C_1_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (42000000U)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
#define I2C_0_ERR_ISR       isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_SCL_PORT      GPIOB
#define I2C_0_SCL_PIN       8
#define I2C_0_SCL_AF        4
#define I2C_0_SCL_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define I2C_0_SDA_PORT      GPIOB
#define I2C_0_SDA_PIN       9
#define I2C_0_SDA_AF        4
#define I2C_0_SDA_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)

/* I2C 1 device configuration */
#define I2C_1_DEV           I2C3
#define I2C_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C3EN)
#define I2C_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C3EN))
#define I2C_1_EVT_IRQ       I2C3_EV_IRQn
#define I2C_1_EVT_ISR       isr_i2c3_ev
#define I2C_1_ERR_IRQ       I2C3_ER_IRQn
#define I2C_1_ERR_ISR       isr_i2c3_er
/* I2C 1 pin configuration */
#define I2C_1_SCL_PORT      GPIOA
#define I2C_1_SCL_PIN       8
#define I2C_1_SCL_AF        4
#define I2C_1_SCL_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define I2C_1_SDA_PORT      GPIOB
#define I2C_1_SDA_PIN       4
#define I2C_1_SDA_AF        9
#define I2C_1_SDA_CLKEN()   (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          3
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          -1/* not configured */
#define GPIO_IRQ_1          -1/* not configured */
#define GPIO_IRQ_2          -1/* not configured */
#define GPIO_IRQ_3          -1/* not configured */
#define GPIO_IRQ_4          -1/* not configured */
#define GPIO_IRQ_5          -1/* not configured */
#define GPIO_IRQ_6          -1/* not configured */
#define GPIO_IRQ_7          -1/* not configured */
#define GPIO_IRQ_8          -1/* not configured */
#define GPIO_IRQ_9          -1/* not configured */
#define GPIO_IRQ_10         -1/* not configured */
#define GPIO_IRQ_11         -1/* not configured */
#define GPIO_IRQ_12         -1/* not configured */
#define GPIO_IRQ_13         GPIO_0
#define GPIO_IRQ_14         -1/* not configured */
#define GPIO_IRQ_15         -1/* not configured */

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOC                   /* Used for user button 1 */
#define GPIO_0_PIN          13
#define GPIO_0_CLK          2                       /* 0: PORT A, 1: B ... */
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC)
#define GPIO_0_IRQ          EXTI15_10_IRQn

/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOC                   /* Used for user button 1 */
#define GPIO_1_PIN          2
#define GPIO_1_CLK          2                       /* 0: PORT A, 1: B ... */
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PC)
#define GPIO_1_IRQ          EXTI2_IRQn

/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOC                   /* Used for user button 1 */
#define GPIO_2_PIN          3
#define GPIO_2_CLK          2                       /* 0: PORT A, 1: B ... */
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PC)
#define GPIO_2_IRQ          EXTI3_IRQn

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
