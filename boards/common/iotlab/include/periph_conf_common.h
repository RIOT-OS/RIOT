/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_iotlab
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the iotlab-m3 board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Clock settings
 *
 * @note    This is auto-generated from
 *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 72MHz */
 #define CLOCK_CORECLOCK      (72000000U)
 /* 0: no external high speed crystal available
  * else: actual crystal frequency [in Hz] */
 #define CLOCK_HSE            (16000000U)
 /* 0: no external low speed crystal available,
  * 1: external crystal available (always 32.768kHz) */
 #define CLOCK_LSE            (1)
 /* peripheral clock setup */
 #define CLOCK_AHB_DIV        RCC_CFGR_HPRE_DIV1
 #define CLOCK_AHB            (CLOCK_CORECLOCK / 1)
 #define CLOCK_APB1_DIV       RCC_CFGR_PPRE1_DIV2     /* max 36MHz */
 #define CLOCK_APB1           (CLOCK_CORECLOCK / 2)
 #define CLOCK_APB2_DIV       RCC_CFGR_PPRE2_DIV1     /* max 72MHz */
 #define CLOCK_APB2           (CLOCK_CORECLOCK / 1)

 /* PLL factors */
 #define CLOCK_PLL_PREDIV     (2)
 #define CLOCK_PLL_MUL        (9)
 /** @} */

/**
 * @name    ADC configuration
 * @{
 */
#define ADC_CONFIG          {       \
    { GPIO_PIN(PORT_A,3), 0, 3  },  \
    { GPIO_UNDEF        , 0, 16 },  \
    { GPIO_UNDEF        , 0, 17 } }

#define ADC_NUMOF           (3)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
#ifdef MODULE_PERIPH_DMA
static const dma_conf_t dma_config[] = {
    { .stream = 3 },    /* DMA1 Channel 4 - USART1_TX */
    { .stream = 5 },    /* DMA1 Channel 6 - USART2_TX */
};

#define DMA_0_ISR  isr_dma1_channel4
#define DMA_1_ISR  isr_dma1_channel6

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
#endif
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM2,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .dev      = TIM3,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus      = APB1,
        .irqn     = TIM3_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin   = GPIO_PIN(PORT_A, 10),
        .tx_pin   = GPIO_PIN(PORT_A, 9),
        .bus      = APB2,
        .irqn     = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = 2
#endif
    },
    {
        .dev      = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin   = GPIO_PIN(PORT_A, 3),
        .tx_pin   = GPIO_PIN(PORT_A, 2),
        .bus      = APB1,
        .irqn     = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 1,
        .dma_chan   = 2
#endif
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_IRQ_PRIO        1

#define RTT_DEV             RTC
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (1)             /* in Hz */
#define RTT_PRESCALER       (0x7fff)        /* run with 1 Hz */
/** @} */

/**
 * @name    I2C configuration
  * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 6),
        .sda_pin        = GPIO_PIN(PORT_B, 7),
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @brief   Shared SPI clock div table
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 36000000Hz */
        7,  /* -> 140625Hz */
        6,  /* -> 281250Hz */
        4,  /* -> 1125000Hz */
        2,  /* -> 4500000Hz */
        1   /* -> 9000000Hz */
    },
    {       /* for APB2 @ 72000000Hz */
        7,  /* -> 281250Hz */
        7,  /* -> 281250Hz */
        5,  /* -> 1125000Hz */
        3,  /* -> 4500000Hz */
        2   /* -> 9000000Hz */
    }
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
