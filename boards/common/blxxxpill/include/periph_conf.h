/*
 * SPDX-FileCopyrightText: 2015 TriaGnoSys GmbH
 * SPDX-FileCopyrightText: 2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_blxxxpill
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the bluepill/blackpill boards
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 *
 */

/* blxxxpill boards provide an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* blxxxpill boards provide an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Real time counter configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (16384)      /* in Hz */
#endif
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = 0, .chan = 0 },
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 0, .chan = 4 },
    { .pin = GPIO_PIN(PORT_A, 5), .dev = 0, .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .dev = 0, .chan = 6 },
    { .pin = GPIO_PIN(PORT_A, 7), .dev = 0, .chan = 7 },
    /* ADC Temperature channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 16 },
    /* ADC VREF channel */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 17 },
    /* The blackpill has a few pins less. PB0 and PB1 are among the GPIOs not
     * exposed due to the lower pincount.
     *
     * Also, this conflicts with PWM. We prefer PWM over ADC here to provide
     * 6 external ADC inputs, and 4 PWM outputs (instead of 8 ADC inputs and
     * 2 PWM outputs). */
#if !defined(BOARD_BLACKPILL_STM32F103C8) \
    && !defined(BOARD_BLACKPILL_STM32F103CB) \
    && !defined(MODULE_PERIPH_PWM)
    { .pin = GPIO_PIN(PORT_B, 0), .dev = 0, .chan = 8 },
    { .pin = GPIO_PIN(PORT_B, 1), .dev = 0, .chan = 9 },
#endif
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 1 },    /* DMA1 Channel 2 - SPI1_RX / USART3_TX */
    { .stream = 2 },    /* DMA1 Channel 3 - SPI1_TX */
    { .stream = 3 },    /* DMA1 Channel 4 - SPI2_RX / USART1_TX */
    { .stream = 4 },    /* DMA1 Channel 5 - SPI2_TX */
    { .stream = 6 },    /* DMA1 Channel 7 - USART2_TX */
};

#define DMA_0_ISR   isr_dma1_channel2
#define DMA_1_ISR   isr_dma1_channel3
#define DMA_2_ISR   isr_dma1_channel4
#define DMA_3_ISR   isr_dma1_channel5
#define DMA_4_ISR   isr_dma1_channel7

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name   Timer configuration
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
    },
    {
        .dev      = TIM4,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .bus      = APB1,
        .irqn     = TIM4_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim3
#define TIMER_2_ISR         isr_tim4

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    QDEC configuration
 * @{
 */

static const qdec_conf_t qdec_config[] = {
    {
        .dev        = TIM4,
        .max        = 0x0000ffff,
        .rcc_mask   = RCC_APB1ENR_TIM4EN,
        .chan       = { { .pin = GPIO_PIN(PORT_B, 6),   .cc_chan = 0 },
                        { .pin = GPIO_PIN(PORT_B, 7),   .cc_chan = 1 } },
        .bus        = APB1,
        .irqn       = TIM4_IRQn,
    },
    /* this conflicts with PWM */
#ifndef MODULE_PERIPH_PWM
    {
        .dev        = TIM3,
        .max        = 0x0000ffff,
        .rcc_mask   = RCC_APB1ENR_TIM3EN,
        .chan       = { { .pin = GPIO_PIN(PORT_B, 4),   .cc_chan = 0 },
                        { .pin = GPIO_PIN(PORT_B, 5),   .cc_chan = 1 } },
        /* by default TIM3 is routed to PA6 (cc_chan 0) and PA7 (cc_chan 1) */
        .remap      = AFIO_MAPR_TIM3_REMAP_1,
        .bus        = APB1,
        .irqn       = TIM3_IRQn,
    },
#endif
    /* this conflicts with UART_DEV(0) */
#ifndef MODULE_PERIPH_UART
    {
        .dev      = TIM1,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 8),     .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_A, 9),     .cc_chan = 1 } },
        .bus      = APB2,
        .irqn     = TIM1_UP_IRQn
    },
#endif
};

#define QDEC_NUMOF           ARRAY_SIZE(qdec_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .bus        = APB2,
        .irqn       = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .bus        = APB1,
        .irqn       = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 4,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
    {
        .dev        = USART3,
        .rcc_mask   = RCC_APB1ENR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_B, 11),
        .tx_pin     = GPIO_PIN(PORT_B, 10),
        .bus        = APB1,
        .irqn       = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    I2C configuration
 * @note    This board may require external pullup resistors for i2c operation.
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C2_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev
#define I2C_1_ISR           isr_i2c2_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        /* by default TIM3 is routed to PA6 (cc_chan 0) and PA7 (cc_chan 1) */
        .remap      = AFIO_MAPR_TIM3_REMAP_1,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B, 4), .cc_chan = 0 },
                        { .pin = GPIO_PIN(PORT_B, 5), .cc_chan = 1 },
#if !defined(BOARD_BLACKPILL_STM32F103C8) \
    && !defined(BOARD_BLACKPILL_STM32F103CB)
                        /* The blackpill has a few pins less. PB0 and PB1 are
                         * among the GPIOs not exposed due to the lower
                         * pincount */
                        { .pin = GPIO_PIN(PORT_B, 0), .cc_chan = 2 },
                        { .pin = GPIO_PIN(PORT_B, 1), .cc_chan = 3 },
#endif
                    },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1,
    },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = GPIO_PIN(PORT_B, 12),
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 3,
        .tx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED,
        .rx_dma   = 2,
        .rx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
#ifndef MODULE_PERIPH_ADC
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4),
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED,
        .rx_dma   = 0,
        .rx_dma_chan = DMA_CHAN_CONFIG_UNSUPPORTED
#endif
    },
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @brief USB device FS configuration
 */
static const stm32_usbdev_fs_config_t stm32_usbdev_fs_config[] = {
    {
        .base_addr  = (uintptr_t)USB,
        .rcc_mask   = RCC_APB1ENR_USBEN,
        .irqn       = USB_LP_CAN1_RX0_IRQn,
        .apb        = APB1,
        .dm         = GPIO_PIN(PORT_A, 11),
        .dp         = GPIO_PIN(PORT_A, 12),
        .af         = GPIO_AF_UNDEF,
        .disconn    = GPIO_UNDEF,
    },
};

/**
 * @brief Interrupt function name mapping
 */
#define USBDEV_ISR             isr_usb_lp_can1_rx0

/**
 * @brief Number of available USB device FS peripherals
 */
#define USBDEV_NUMOF           ARRAY_SIZE(stm32_usbdev_fs_config)

#ifdef __cplusplus
}
#endif

/** @} */
