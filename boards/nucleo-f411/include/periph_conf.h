/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f411
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the nucleo-f411 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
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
#define CLOCK_CORECLOCK     (100000000U)        /* desired core clock frequency */

/* the actual PLL values are automatically generated */
#define CLOCK_PLL_M         (CLOCK_HSE / 1000000)
#define CLOCK_PLL_N         ((CLOCK_CORECLOCK / 1000000) * 2)
#define CLOCK_PLL_P         (2U)
#define CLOCK_PLL_Q         (CLOCK_PLL_N / 48)
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY_5WS

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM5,
        .max      = 0xffffffff,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim5
#define TIMER_1_ISR         isr_tim4

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A,3),
        .tx_pin     = GPIO_PIN(PORT_A,2),
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
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 3
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_A, 12),
        .tx_pin     = GPIO_PIN(PORT_A, 11),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef UART_USE_DMA
        .dma_stream = 6,
        .dma_chan   = 2
#endif
    }
};

/* assign ISR vector names */
#define UART_0_ISR          isr_usart2
#define UART_0_DMA_ISR      isr_dma1_stream6
#define UART_1_ISR          isr_usart1
#define UART_1_DMA_ISR      isr_dma1_stream6
#define UART_2_ISR          isr_usart6
#define UART_2_DMA_ISR      isr_dma1_stream6

/* deduct number of defined UART interfaces */
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/** @brief   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 15)         , .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 3)  /* D3 */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 10) /* D6 */, .cc_chan = 2 },
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB1
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4) /* D5 */, .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7) /* D9 */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8),          .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_C, 9),          .cc_chan = 3 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */

/**
 * @brief   SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 50000000Hz */
        7,  /* -> 195312Hz */
        6,  /* -> 390625Hz */
        5,  /* -> 781250Hz */
        2,  /* -> 6250000Hz */
        1   /* -> 12500000Hz */
    },
    {       /* for APB2 @ 100000000Hz */
        7,  /* -> 390625Hz */
        7,  /* -> 390625Hz */
        6,  /* -> 781250Hz */
        3,  /* -> 6250000Hz */
        2   /* -> 12500000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4),
        .af       = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF           (1U)
#define I2C_0_EN            1
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
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
#define ADC_NUMOF          (0)
/** @} */

/**
 * @brief   DAC configuration
 * @{
 */
#define DAC_NUMOF           (0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
