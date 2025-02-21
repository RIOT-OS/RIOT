/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-wl55jc board
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 * @author      Hauke Petersen <devel@haukepetersen.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            1
#endif

/* This board provides a 32MHz HSE oscillator  */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            1
#endif

#ifndef CONFIG_CLOCK_HSE
#define CONFIG_CLOCK_HSE                       MHZ(32)
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_B, 7),
        .tx_pin     = GPIO_PIN(PORT_B, 6),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          (isr_lpuart1)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SUBGHZSPI, /* Internally connected to Sub-GHz radio Modem  */
        .mosi_pin = GPIO_UNDEF,
        .miso_pin = GPIO_UNDEF,
        .sclk_pin = GPIO_UNDEF,
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF_UNDEF,
        .miso_af  = GPIO_AF_UNDEF,
        .sclk_af  = GPIO_AF_UNDEF,
        .cs_af    = GPIO_AF_UNDEF,
        .rccmask  = RCC_APB3ENR_SUBGHZSPIEN,
        .apbbus   = APB3,
    },
/* SUBGHZ DEBUG PINS use the SPI1 pins */
#if !IS_ACTIVE(CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG)
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
    }
#endif
};

#define SPI_NUMOF               ARRAY_SIZE(spi_config)
/**
 * @brief   Provide ARDUINO_SPI_D11D12D13 explicitly, as the first SPI
 *          interface is connected to the radio.
 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(1)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 12),
        .sda_pin        = GPIO_PIN(PORT_A, 11),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C2EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C2SEL_1,  /* HSI (16 MHz) */
        .irqn           = I2C2_ER_IRQn,
    }
};

#define I2C_1_ISR           isr_i2c2_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

static const adc_conf_t adc_config[] = {
    {GPIO_PIN(PORT_B, 1),  .chan = 5},  /* ADC1_IN5 */
    {GPIO_PIN(PORT_B, 2),  .chan = 4},  /* ADC1_IN4 */
    {GPIO_PIN(PORT_A, 10), .chan = 6},  /* ADC1_IN6 */
    {GPIO_PIN(PORT_B, 4),  .chan = 3},  /* ADC1_IN3 */
    {GPIO_PIN(PORT_B, 14), .chan = 1},  /* ADC1_IN1 */
    {GPIO_PIN(PORT_B, 13), .chan = 0},  /* ADC1_IN0 */
    {GPIO_UNDEF,           .chan = 14}, /* VBAT see datasheet point 3.20.3 */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
