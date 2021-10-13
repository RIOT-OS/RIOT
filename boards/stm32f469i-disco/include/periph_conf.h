/*
 * Copyright (C) 2021 luisan00
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup   boards_stm32f469i-disco
 * @{
 * @file
 * @brief     Peripheral MCU configuration for the STM32F469I-DISCO
 * @author    Luis A. Ruiz <luisan00@hotmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board provides a LSE, marked as X3 on the board */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE 1
#endif

/* This board provides a HSE, marked as X2 on the board */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE 1
#endif

/* The HSE provides a 8MHz clock */
#define CLOCK_HSE MHZ(8)

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"
#include "cfg_usb_otg_fs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    DMA streams configuration
 * @{
 */
static const dma_conf_t dma_config[] = {
    {.stream = 14}, /* DMA2 Stream 6 - SPI TX */
    {.stream = 13}, /* DMA2 Stream 5 - SPI RX */
};
#define DMA_0_ISR isr_dma2_stream6
#define DMA_1_ISR isr_dma2_stream5
#define DMA_NUMOF ARRAY_SIZE(dma_config)
/** @} */

/**
 * @name    UART configuration
 * UART_DEV(0): Is used as Virtual Com Port for the STLINK, not available pyshically
 * UART_DEV(1): Available on CN12 and Arduino(R) expansion connector CN7
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin = GPIO_PIN(PORT_B, 11),
        .tx_pin = GPIO_PIN(PORT_B, 10),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB1,
        .irqn = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = DMA_STREAM_UNDEF,
        .dma_chan = UINT8_MAX
#endif
    },
    {
        .dev = USART6,
        .rcc_mask = RCC_APB2ENR_USART6EN,
        .rx_pin = GPIO_PIN(PORT_C, 7),
        .tx_pin = GPIO_PIN(PORT_C, 6),
        .rx_af = GPIO_AF8,
        .tx_af = GPIO_AF8,
        .bus = APB2,
        .irqn = USART6_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma = DMA_STREAM_UNDEF,
        .dma_chan = UINT8_MAX
#endif
    },
};
#define UART_0_ISR (isr_usart3)
#define UART_1_ISR (isr_usart6)
#define UART_NUMOF ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   I2C configuration
 *
 * DEV_I2C(0): Available on the connector CN11
 * and the Arduino(R) connector CN5 ONLY if we have the next configuration
 * on the solder bridges; open SB10 and SB12, close SB9 and SB11.
 * For more information see the user manual.
 *
 * DEV_I2C(1): Is not physically available but
 * it's used for the audio DAC-SAI, with address 0x94
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C1,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 8),
        .sda_pin = GPIO_PIN(PORT_B, 9),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C1EN,
        .clk = CLOCK_APB1,
        .irqn = I2C1_EV_IRQn,
    },
    {
        .dev = I2C2,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_H, 4),
        .sda_pin = GPIO_PIN(PORT_H, 5),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR_I2C2EN,
        .clk = CLOCK_APB1,
        .irqn = I2C2_EV_IRQn,
    },
};
#define I2C_0_ISR isr_i2c1_ev
#define I2C_1_ISR isr_i2c2_ev
#define I2C_NUMOF ARRAY_SIZE(i2c_config)
/** @}*/

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_D, 3),
        .cs_pin = GPIO_PIN(PORT_H, 6),
        .mosi_af = GPIO_AF5,
        .miso_af = GPIO_AF5,
        .sclk_af = GPIO_AF5,
        .cs_af = GPIO_AF5,
        .rccmask = RCC_APB1ENR_I2C2EN,
        .apbbus = APB1,
#ifdef MODULE_PERIPH_DMA
        .tx_dma = 0,
        .tx_dma_chan = 6,
        .rx_dma = 1,
        .rx_dma_chan = 6,
#endif
    },
};
#define SPI_NUMOF ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 6), .cc_chan = 0}, /* D6 */
            {.pin = GPIO_PIN(PORT_B, 5), .cc_chan = 1}, /* CN12-P9 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
        },
        .af = GPIO_AF2,
        .bus = APB1,
    },
    {
        .dev = TIM5,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 1), .cc_chan = 1}, /* D5 */
            {.pin = GPIO_PIN(PORT_A, 2), .cc_chan = 2}, /* D3 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},          /* unused */
        },
        .af = GPIO_AF2,
        .bus = APB1,
    },
    {
        .dev = TIM12,
        .rcc_mask = RCC_APB1ENR_TIM12EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_H, 6), .cc_chan = 0},     /* D10 */
            {.pin = GPIO_PIN(PORT_B, 15), .cc_chan = 1},    /* D11 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
        },
        .af = GPIO_AF9,
        .bus = APB1,
    },
    {
        .dev = TIM14,
        .rcc_mask = RCC_APB1ENR_TIM14EN,
        .chan = {
            {.pin = GPIO_PIN(PORT_A, 7), .cc_chan = 0},     /* D9 */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
            {.pin = GPIO_UNDEF, .cc_chan = 0},              /* unused */
        },
        .af = GPIO_AF9,
        .bus = APB1,
    },
};
#define PWM_NUMOF   ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name ADC configuration
 *
 * Not all ADCs are configured, by now, only the 6 ones available
 * on the Arduino(R) connector A0-A5
 * @{
 */
static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_B, 1), 2, 9},
    {GPIO_PIN(PORT_C, 2), 2, 12},
    {GPIO_PIN(PORT_C, 3), 2, 13},
    {GPIO_PIN(PORT_C, 4), 2, 14},
    {GPIO_PIN(PORT_C, 5), 2, 15},
    {GPIO_PIN(PORT_A, 4), 2, 4},
};
#define ADC_NUMOF   ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
