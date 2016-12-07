/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32f3discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F3discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define CLOCK_HSE           (8000000U)          /* external oscillator */
#define CLOCK_CORECLOCK     (72000000U)         /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_MUL       (CLOCK_CORECLOCK / CLOCK_HSE)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_2

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_CONFIG {                \
    { GPIO_PIN(PORT_A, 4), 0, 0 },  \
}

#define DAC_NUMOF           (1)
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A,  9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @brief   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .pins     = { GPIO_PIN(PORT_C, 6), GPIO_PIN(PORT_C, 7),
                      GPIO_PIN(PORT_C, 8), GPIO_PIN(PORT_C, 9) },
        .af       = GPIO_AF2,
        .chan     = 4,
        .bus      = APB1
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .pins     = { GPIO_PIN(PORT_D, 12), GPIO_PIN(PORT_D, 13),
                      GPIO_PIN(PORT_D, 14), GPIO_PIN(PORT_D, 15) },
        .af       = GPIO_AF2,
        .chan     = 4,
        .bus      = APB1
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
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
#define SPI_0_DEV               SPI1
#define SPI_0_CLKEN()           (periph_clk_en(APB2, RCC_APB2ENR_SPI1EN))
#define SPI_0_CLKDIS()          (periph_clk_dis(APB2, RCC_APB2ENR_SPI1EN))
#define SPI_0_IRQ               SPI1_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          GPIOA
#define SPI_0_SCK_PIN           5
#define SPI_0_SCK_AF            5
#define SPI_0_SCK_PORT_CLKEN()  (periph_clk_en(AHB, RCC_AHBENR_GPIOAEN))
#define SPI_0_MISO_PORT         GPIOA
#define SPI_0_MISO_PIN          6
#define SPI_0_MISO_AF           5
#define SPI_0_MISO_PORT_CLKEN() (periph_clk_en(AHB, RCC_AHBENR_GPIOAEN))
#define SPI_0_MOSI_PORT         GPIOA
#define SPI_0_MOSI_PIN          7
#define SPI_0_MOSI_AF           5
#define SPI_0_MOSI_PORT_CLKEN() (periph_clk_en(AHB, RCC_AHBENR_GPIOAEN))

/* SPI 1 device config */
#define SPI_1_DEV               SPI3
#define SPI_1_CLKEN()           (periph_clk_en(APB1, RCC_APB1ENR_SPI3EN))
#define SPI_1_CLKDIS()          (periph_clk_dis(APB1, RCC_APB1ENR_SPI3EN))
#define SPI_1_IRQ               SPI3_IRQn
#define SPI_1_IRQ_HANDLER       isr_spi3
/* SPI 1 pin configuration */
#define SPI_1_SCK_PORT          GPIOC
#define SPI_1_SCK_PIN           10
#define SPI_1_SCK_AF            6
#define SPI_1_SCK_PORT_CLKEN()  (periph_clk_en(AHB, RCC_AHBENR_GPIOCEN))
#define SPI_1_MISO_PORT         GPIOC
#define SPI_1_MISO_PIN          11
#define SPI_1_MISO_AF           6
#define SPI_1_MISO_PORT_CLKEN() (periph_clk_en(AHB, RCC_AHBENR_GPIOCEN))
#define SPI_1_MOSI_PORT         GPIOC
#define SPI_1_MOSI_PIN          12
#define SPI_1_MOSI_AF           6
#define SPI_1_MOSI_PORT_CLKEN() (periph_clk_en(AHB, RCC_AHBENR_GPIOCEN))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (2U)
#define I2C_0_EN            1
#define I2C_1_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (36000000U)

/* I2C 0 device configuration */
#define I2C_0_DEV           I2C1
#define I2C_0_CLKEN()       (periph_clk_en(APB1, RCC_APB1ENR_I2C1EN))
#define I2C_0_CLKDIS()      (periph_clk_dis(APB1, RCC_APB1ENR_I2C1EN))
#define I2C_0_EVT_IRQ       I2C1_EV_IRQn
#define I2C_0_EVT_ISR       isr_i2c1_ev
#define I2C_0_ERR_IRQ       I2C1_ER_IRQn
#define I2C_0_ERR_ISR       isr_i2c1_er
/* I2C 0 pin configuration */
#define I2C_0_SCL_PORT      GPIOB
#define I2C_0_SCL_PIN       6
#define I2C_0_SCL_AF        4
#define I2C_0_SCL_CLKEN()   (periph_clk_en(AHB, RCC_AHBENR_GPIOBEN))
#define I2C_0_SDA_PORT      GPIOB
#define I2C_0_SDA_PIN       7
#define I2C_0_SDA_AF        4
#define I2C_0_SDA_CLKEN()   (periph_clk_en(AHB, RCC_AHBENR_GPIOBEN))

/* I2C 1 device configuration */
#define I2C_1_DEV           I2C2
#define I2C_1_CLKEN()       (periph_clk_en(APB1, RCC_APB1ENR_I2C2EN))
#define I2C_1_CLKDIS()      (periph_clk_dis(APB1, RCC_APB1ENR_I2C2EN))
#define I2C_1_EVT_IRQ       I2C2_EV_IRQn
#define I2C_1_EVT_ISR       isr_i2c2_ev
#define I2C_1_ERR_IRQ       I2C2_ER_IRQn
#define I2C_1_ERR_ISR       isr_i2c2_er
/* I2C 1 pin configuration */
#define I2C_1_SCL_PORT      GPIOF
#define I2C_1_SCL_PIN       1
#define I2C_1_SCL_AF        4
#define I2C_1_SCL_CLKEN()   (periph_clk_en(AHB, RCC_AHBENR_GPIOFEN))
#define I2C_1_SDA_PORT      GPIOF
#define I2C_1_SDA_PIN       0
#define I2C_1_SDA_AF        4
#define I2C_1_SDA_CLKEN()   (periph_clk_en(AHB, RCC_AHBENR_GPIOFEN))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
