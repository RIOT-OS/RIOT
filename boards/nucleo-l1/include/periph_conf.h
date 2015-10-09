/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nucleo-l1
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-l1 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
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
 **/
#define CLOCK_HSI           (16000000U)             /* frequency of internal oscillator */
#define CLOCK_CORECLOCK     (32000000U)             /* targeted core clock frequency */
/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / CLOCK_PLL_DIV * CLOCK_PLL_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL4
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* device, RCC bit, IRQ bit */
    {TIM5, 3, TIM5_IRQn},
};
/* interrupt routines */
#define TIMER_0_ISR         (isr_tim5)
/* number of defined timers */
#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief UART configuration
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          USART2
#define UART_0_CLKEN()      (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define UART_0_CLK          (CLOCK_CORECLOCK)   /* UART clock runs with 32MHz (F_CPU / 1) */
#define UART_0_IRQ          USART2_IRQn
#define UART_0_ISR          isr_usart2
#define UART_0_BUS_FREQ     32000000
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_PORT_CLKEN() (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define UART_0_RX_PIN       3
#define UART_0_TX_PIN       2
#define UART_0_AF           7

/**
 * @brief GPIO configuration
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
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_13
#define GPIO_IRQ_1          GPIO_14
#define GPIO_IRQ_2          GPIO_7
#define GPIO_IRQ_3          GPIO_0
#define GPIO_IRQ_4          GPIO_5
#define GPIO_IRQ_5          GPIO_12
#define GPIO_IRQ_6          GPIO_11
#define GPIO_IRQ_7          GPIO_1
#define GPIO_IRQ_8          GPIO_3
#define GPIO_IRQ_9          GPIO_2
#define GPIO_IRQ_10         GPIO_4
#define GPIO_IRQ_11         GPIO_6
#define GPIO_IRQ_12         GPIO_15
#define GPIO_IRQ_13         GPIO_8
#define GPIO_IRQ_14         GPIO_9
#define GPIO_IRQ_15         GPIO_10

/* GPIO channel 0 config */
#define GPIO_0_PORT         GPIOA                   /* Used for user button 1 */
#define GPIO_0_PIN          3
#define GPIO_0_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_0_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA)
#define GPIO_0_IRQ          EXTI3_IRQn
/* GPIO channel 1 config */
#define GPIO_1_PORT         GPIOC
#define GPIO_1_PIN          7
#define GPIO_1_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_1_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PC)
#define GPIO_1_IRQ          EXTI9_5_IRQn
/* GPIO channel 2 config */
#define GPIO_2_PORT         GPIOA
#define GPIO_2_PIN          9
#define GPIO_2_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_2_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PA)
#define GPIO_2_IRQ          EXTI9_5_IRQn
/* GPIO channel 3 config */
#define GPIO_3_PORT         GPIOA
#define GPIO_3_PIN          8
#define GPIO_3_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_3_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA)
#define GPIO_3_IRQ          EXTI9_5_IRQn
/* GPIO channel 4 config */
#define GPIO_4_PORT         GPIOB
#define GPIO_4_PIN          10
#define GPIO_4_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_4_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PB)
#define GPIO_4_IRQ          EXTI15_10_IRQn
/* GPIO channel 5 config */
#define GPIO_5_PORT         GPIOB
#define GPIO_5_PIN          4
#define GPIO_5_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_5_EXTI_CFG()   (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PB)
#define GPIO_5_IRQ          EXTI4_IRQn
/* GPIO channel 6 config */
#define GPIO_6_PORT         GPIOC
#define GPIO_6_PIN          11
#define GPIO_6_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_6_EXTI_CFG()   (SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC)
#define GPIO_6_IRQ          EXTI15_10_IRQn
/* GPIO channel 7 config */
#define GPIO_7_PORT         GPIOC
#define GPIO_7_PIN          2
#define GPIO_7_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_7_EXTI_CFG()   (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PC)
#define GPIO_7_IRQ          EXTI2_IRQn
/* GPIO channel 8 config */
#define GPIO_8_PORT         GPIOA
#define GPIO_8_PIN          13
#define GPIO_8_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_8_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PA)
#define GPIO_8_IRQ          EXTI15_10_IRQn
/* GPIO channel 9 config */
#define GPIO_9_PORT         GPIOA
#define GPIO_9_PIN          14
#define GPIO_9_CLKEN()      (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_9_EXTI_CFG()   (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI14_PA)
#define GPIO_9_IRQ          EXTI15_10_IRQn
/* GPIO channel 10 config */
#define GPIO_10_PORT        GPIOA
#define GPIO_10_PIN         15
#define GPIO_10_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_10_EXTI_CFG()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PA)
#define GPIO_10_IRQ         EXTI15_10_IRQn
/* GPIO channel 11 config */
#define GPIO_11_PORT        GPIOB   /* SPI CS Pin */
#define GPIO_11_PIN         6
#define GPIO_11_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define GPIO_11_EXTI_CFG()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PB)
#define GPIO_11_IRQ         EXTI9_5_IRQn
/* GPIO channel 12 config */
#define GPIO_12_PORT        GPIOC
#define GPIO_12_PIN         5
#define GPIO_12_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_12_EXTI_CFG()  (SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PC)
#define GPIO_12_IRQ         EXTI9_5_IRQn
/* GPIO channel 13 config */
#define GPIO_13_PORT        GPIOA
#define GPIO_13_PIN         0
#define GPIO_13_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_13_EXTI_CFG()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA)
#define GPIO_13_IRQ         EXTI0_IRQn
/* GPIO channel 14 config */
#define GPIO_14_PORT        GPIOA
#define GPIO_14_PIN         1
#define GPIO_14_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define GPIO_14_EXTI_CFG()  (SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA)
#define GPIO_14_IRQ         EXTI1_IRQn
/* GPIO channel 15 config */
#define GPIO_15_PORT        GPIOC
#define GPIO_15_PIN         12
#define GPIO_15_CLKEN()     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)
#define GPIO_15_EXTI_CFG()  (SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC)
#define GPIO_15_IRQ         EXTI15_10_IRQn

/**
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device configuration */
#define SPI_0_DEV           SPI1
#define SPI_0_CLKEN()       (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()      (RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ           SPI1_IRQn
#define SPI_0_ISR           isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOAEN)
#define SPI_0_PORT          GPIOA
#define SPI_0_PIN_SCK       5
#define SPI_0_PIN_MOSI      7
#define SPI_0_PIN_MISO      6
#define SPI_0_PIN_AF        5
/** @} */

/**
 * @name I2C configuration
  * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C1EN)
#define I2C_0_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
#define I2C_0_ERR_ISR       isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_PORT_CLKEN()  (RCC->AHBENR |= RCC_AHBENR_GPIOBEN)
#define I2C_0_PORT          GPIOB
#define I2C_0_SCL_PIN       8
#define I2C_0_SCL_AF        4
#define I2C_0_SDA_PIN       9
#define I2C_0_SDA_AF        4
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
