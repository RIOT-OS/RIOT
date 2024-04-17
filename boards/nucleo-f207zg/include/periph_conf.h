/*
 * Copyright (C) 2016-2017  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-f207zg
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the nucleo-f207zg board
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.fr>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb8_pb9.h"
#include "cfg_usb_otg_fs.h"
#include "mii.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    { .stream = 10 },   /* DMA2 Stream 2 - SPI1_RX */
    { .stream = 11 },   /* DMA2 Stream 3 - SPI1_TX */
    { .stream = 3 },    /* DMA1 Stream 3 - SPI2_RX/USART3_TX */
    { .stream = 4 },    /* DMA1 Stream 4 - SPI2_TX */
    { .stream = 14 },   /* DMA2 Stream 6 - USART6_TX */
    { .stream = 6 },    /* DMA1 Stream 6 - USART2_TX */
    { .stream = 8 },    /* DMA2 Stream 0 - ETH_TX */
};

#define DMA_0_ISR  isr_dma2_stream2
#define DMA_1_ISR  isr_dma2_stream3
#define DMA_2_ISR  isr_dma1_stream3
#define DMA_3_ISR  isr_dma1_stream4
#define DMA_4_ISR  isr_dma2_stream6
#define DMA_5_ISR  isr_dma1_stream6
#define DMA_6_ISR  isr_dma2_stream0

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_E,  9) /* D6 */, .cc_chan = 0},
                      { .pin = GPIO_PIN(PORT_E, 11) /* D5 */, .cc_chan = 1},
                      { .pin = GPIO_PIN(PORT_E, 13) /* D3 */, .cc_chan = 2},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0} },
        .af       = GPIO_AF1,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_D, 15) /* D9 */, .cc_chan = 3},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0},
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0} },
        .af       = GPIO_AF2,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .base     = periph_timer2,
        .dev      = TIM2,
        .max      = 0xffffffff,
        .bus      = APB1,
        .irqn     = TIM2_IRQn
    },
    {
        .base     = periph_timer5,
        .dev      = TIM5,
        .max      = 0xffffffff,
        .bus      = APB1,
        .irqn     = TIM5_IRQn
    }
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim5

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
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
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 4
#endif
    },
    {
        .dev        = USART6,
        .rcc_mask   = RCC_APB2ENR_USART6EN,
        .rx_pin     = GPIO_PIN(PORT_G, 9),
        .tx_pin     = GPIO_PIN(PORT_G, 14),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB2,
        .irqn       = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 4,
        .dma_chan   = 5
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
#ifdef MODULE_PERIPH_DMA
        .dma        = 5,
        .dma_chan   = 4
#endif
    },
};

#define UART_0_ISR          (isr_usart3)
#define UART_1_ISR          (isr_usart6)
#define UART_2_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = 3,
        .rx_dma   = 0,
        .rx_dma_chan = 3,
#endif
    },
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_C, 2),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = GPIO_PIN(PORT_B, 12),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 3,
        .tx_dma_chan = 0,
        .rx_dma   = 2,
        .rx_dma_chan = 0,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @brief   ADC configuration
 *
 * Note that we do not configure all ADC channels,
 * and not in the STM32F207ZG order.  Instead, we
 * just define 6 ADC channels, for the Nucleo
 * Arduino header pins A0-A5 and the internal VBAT channel.
 *
 * To find appropriate device and channel find in the
 * board manual, table showing pin assignments and
 * information about ADC - a text similar to ADC[X]_IN[Y],
 * where:
 * [X] - describes used device - indexed from 0,
 * for example ADC1_IN10 is device 0,
 * [Y] - describes used channel - indexed from 1,
 * for example ADC1_IN10 is channel 10
 *
 * For Nucleo-F207ZG this information is in board manual,
 * Table 13, page 37.
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 3), .dev = 0, .chan =  3 }, /* ADC123_IN3  */
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 0, .chan = 10 }, /* ADC123_IN10 */
    { .pin = GPIO_PIN(PORT_C, 3), .dev = 0, .chan = 13 }, /* ADC123_IN13 */
    { .pin = GPIO_PIN(PORT_F, 3), .dev = 2, .chan =  9 }, /* ADC3_IN9  */
    { .pin = GPIO_PIN(PORT_F, 5), .dev = 2, .chan = 15 }, /* ADC3_IN15   */
    { .pin = GPIO_PIN(PORT_F, 10), .dev = 2, .chan = 8 }, /* ADC3_IN8    */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 18 }, /* VBAT */
};

/**
 * @brief VBAT ADC line
 */
#define VBAT_ADC            ADC_LINE(6)

/**
 * @brief Number of ADC devices
 */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)

/** @} */

/**
 * @name ETH configuration
 * @{
 */
static const eth_conf_t eth_config = {
    .mode = RMII,
    .speed = MII_BMCR_SPEED_100 | MII_BMCR_FULL_DPLX,
    .dma = 6,
    .dma_chan = 8,
    .phy_addr = 0x00,
    .pins = {
        GPIO_PIN(PORT_G, 13),
        GPIO_PIN(PORT_B, 13),
        GPIO_PIN(PORT_G, 11),
        GPIO_PIN(PORT_C, 4),
        GPIO_PIN(PORT_C, 5),
        GPIO_PIN(PORT_A, 7),
        GPIO_PIN(PORT_C, 1),
        GPIO_PIN(PORT_A, 2),
        GPIO_PIN(PORT_A, 1),
    }
};

#define ETH_DMA_ISR        isr_dma2_stream0

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
