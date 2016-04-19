/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f103
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-f103 board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
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
#define CLOCK_HSE           (8000000U)              /* external oscillator */
#define CLOCK_CORECLOCK     (72000000U)             /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLXTPRE_HSE  /* not divided */
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMULL9

/* AHB, APB1, APB2 dividers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2    /* max 36 MHz (!) */

/* resulting bus clocks */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2          (CLOCK_CORECLOCK)

/* Flash latency */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_2    /* for >= 72 MHz */
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (0)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev     = USART2,
        .rx_pin  = GPIO_PIN(PORT_A, 3),
        .tx_pin  = GPIO_PIN(PORT_A, 2),
        .rcc_pin = RCC_APB1ENR_USART2EN,
        .bus     = APB1,
        .irqn    = USART2_IRQn
    },
    {
        .dev     = USART1,
        .rx_pin  = GPIO_PIN(PORT_A, 10),
        .tx_pin  = GPIO_PIN(PORT_A, 9),
        .rcc_pin = RCC_APB2ENR_USART1EN,
        .bus     = APB2,
        .irqn    = USART1_IRQn
    },
    {
        .dev     = USART3,
        .rx_pin  = GPIO_PIN(PORT_B, 11),
        .tx_pin  = GPIO_PIN(PORT_B, 10),
        .rcc_pin = RCC_APB1ENR_USART3EN,
        .bus     = APB1,
        .irqn    = USART3_IRQn
    }
};

#define UART_0_ISR          isr_usart2
#define UART_1_ISR          isr_usart1
#define UART_2_ISR          isr_usart3

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (2U)
#define I2C_0_EN            1
#define I2C_1_EN            0
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
#define I2C_0_SCL_PIN       GPIO_PIN(PORT_B, 8) /* remapped */
#define I2C_0_SDA_PIN       GPIO_PIN(PORT_B, 9) /* remapped */

/* I2C 1 device configuration */
#define I2C_1_DEV           I2C2
#define I2C_1_CLKEN()       (RCC->APB1ENR |= RCC_APB1ENR_I2C2EN)
#define I2C_1_CLKDIS()      (RCC->APB1ENR &= ~(RCC_APB1ENR_I2C2EN))
#define I2C_1_EVT_IRQ       I2C2_EV_IRQn
#define I2C_1_EVT_ISR       isr_i2c2_ev
#define I2C_1_ERR_IRQ       I2C2_ER_IRQn
#define I2C_1_ERR_ISR       isr_i2c2_er
/* I2C 1 pin configuration */
#define I2C_1_SCL_PIN       GPIO_PIN(PORT_B, 10)
#define I2C_1_SDA_PIN       GPIO_PIN(PORT_B, 11)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            0
#define SPI_IRQ_PRIO        1

/* SPI 0 device config */
#define SPI_0_DEV               SPI1
#define SPI_0_CLKEN()           (RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_0_CLKDIS()          (RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN)
#define SPI_0_IRQ               SPI1_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi1
#define SPI_0_BUS_DIV           1

/* SPI 0 pin configuration */
#define SPI_0_CLK_PIN           GPIO_PIN(PORT_A, 5)
#define SPI_0_MISO_PIN          GPIO_PIN(PORT_A, 6)
#define SPI_0_MOSI_PIN          GPIO_PIN(PORT_A, 7)

/* SPI 1 device config */
#define SPI_1_DEV               SPI2
#define SPI_1_CLKEN()           (RCC->APB1ENR |= RCC_APB1ENR_SPI2EN)
#define SPI_1_CLKDIS()          (RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN)
#define SPI_1_IRQ               SPI2_IRQn
#define SPI_1_IRQ_HANDLER       isr_spi2
#define SPI_1_BUS_DIV           1
/* SPI 1 pin configuration */
#define SPI_1_CLK_PIN           GPIO_PIN(PORT_B, 13)
#define SPI_1_MISO_PIN          GPIO_PIN(PORT_B, 14)
#define SPI_1_MOSI_PIN          GPIO_PIN(PORT_B, 15)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
