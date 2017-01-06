/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f207
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the nucleo-f207 board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 * @author      Toon Stegen <toon.stegen@altran.com>
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
#define CLOCK_CORECLOCK     (120000000U)        /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_M         (CLOCK_HSE / 1000000)
#define CLOCK_PLL_N         ((CLOCK_CORECLOCK / 1000000) * 2)
#define CLOCK_PLL_P         (2U)
#define CLOCK_PLL_Q         (CLOCK_PLL_N / 48)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_5WS

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)
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
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
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
    },
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    },
    {
        .dev      = TIM4,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .bus      = APB1,
        .irqn     = TIM4_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim5
#define TIMER_2_ISR         isr_tim3
#define TIMER_3_ISR         isr_tim4

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */
/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_D, 9),
        .tx_pin     = GPIO_PIN(PORT_D, 8),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 3,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_D, 6),
        .tx_pin     = GPIO_PIN(PORT_D, 5),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A,  9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 7,
        .dma_chan   = 4
#endif
    }
};

#define UART_0_ISR          (isr_usart3)
#define UART_0_DMA_ISR      (isr_dma1_stream3)

#define UART_1_ISR          (isr_usart2)
#define UART_1_DMA_ISR      (isr_dma1_stream6)

#define UART_2_ISR          (isr_usart1)
#define UART_2_DMA_ISR      (isr_dma1_stream7)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
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
#define SPI_0_BUS_DIV           1   /* 1 -> SPI bus runs with half CPU clock, 0 -> quarter CPU clock */
#define SPI_0_IRQ               SPI1_IRQn
#define SPI_0_IRQ_HANDLER       isr_spi1
/* SPI 0 pin configuration */
#define SPI_0_SCK_PORT          GPIOA       /* A5 pin is shared with the green LED. */
#define SPI_0_SCK_PIN           5
#define SPI_0_SCK_AF            5
#define SPI_0_SCK_PORT_CLKEN()  (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOAEN))
#define SPI_0_MISO_PORT         GPIOA
#define SPI_0_MISO_PIN          6
#define SPI_0_MISO_AF           5
#define SPI_0_MISO_PORT_CLKEN() (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOAEN))
#define SPI_0_MOSI_PORT         GPIOA
#define SPI_0_MOSI_PIN          7
#define SPI_0_MOSI_AF           5
#define SPI_0_MOSI_PORT_CLKEN() (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOAEN))

/* SPI 1 device config */
#define SPI_1_DEV               SPI2
#define SPI_1_CLKEN()           (periph_clk_en(APB1, RCC_APB1ENR_SPI2EN))
#define SPI_1_CLKDIS()          (periph_clk_dis(APB1, RCC_APB1ENR_SPI2EN))
#define SPI_1_BUS_DIV           0   /* 1 -> SPI bus runs with half CPU clock, 0 -> quarter CPU clock */
#define SPI_1_IRQ               SPI2_IRQn
#define SPI_1_IRQ_HANDLER       isr_spi2
/* SPI 1 pin configuration */
#define SPI_1_SCK_PORT          GPIOB
#define SPI_1_SCK_PIN           3
#define SPI_1_SCK_AF            5
#define SPI_1_SCK_PORT_CLKEN()  (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOBEN))
#define SPI_1_MISO_PORT         GPIOB
#define SPI_1_MISO_PIN          4
#define SPI_1_MISO_AF           5
#define SPI_1_MISO_PORT_CLKEN() (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOBEN))
#define SPI_1_MOSI_PORT         GPIOB
#define SPI_1_MOSI_PIN          5
#define SPI_1_MOSI_AF           5
#define SPI_1_MOSI_PORT_CLKEN() (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOBEN))
/** @} */


/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
#define I2C_IRQ_PRIO        1
#define I2C_APBCLK          (CLOCK_APB1)

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
#define I2C_0_SCL_PIN       8
#define I2C_0_SCL_AF        4
#define I2C_0_SCL_PULLUP    0
#define I2C_0_SCL_CLKEN()   (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOBEN))
#define I2C_0_SDA_PORT      GPIOB
#define I2C_0_SDA_PIN       9
#define I2C_0_SDA_AF        4
#define I2C_0_SDA_PULLUP    0
#define I2C_0_SDA_CLKEN()   (periph_clk_en(AHB1, RCC_AHB1ENR_GPIOBEN))
/** @} */

/**
 * @brief   ADC configuration
 *
 * We need to define the following fields:
 * PIN, device (ADCx), channel
 * @{
 */
#define ADC_CONFIG {              \
    {GPIO_PIN(PORT_A, 4), 0, 0},  \
    {GPIO_PIN(PORT_A, 5), 1, 0}  \
}
#define ADC_NUMOF          (2)

/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

/**
 * @brief   RTC configuration
 * @{
 */
#define RTC_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
