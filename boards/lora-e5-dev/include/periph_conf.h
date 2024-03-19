/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lora-e5-dev
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the LoRa-E5 Development Board
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
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
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR1_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART2_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB1ENR2_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_C, 1),
        .tx_pin     = GPIO_PIN(PORT_C, 0),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB12,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0, /* Use APB clock */
    },
};

#define UART_0_ISR          isr_usart1
#define UART_1_ISR          isr_usart2
#define UART_2_ISR          isr_lpuart1

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
#if !IS_ACTIVE(CONFIG_STM32_WLX5XX)
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_A, 10),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = SPI_CS_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR1_SPI2EN,
        .apbbus   = APB1,
    }
#endif
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 15),
        .sda_pin        = GPIO_PIN(PORT_A, 15),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C2EN,
        .rcc_sw_mask    = RCC_CCIPR_I2C2SEL_1,      /* HSI (16 MHz) */
        .irqn           = I2C2_ER_IRQn,
    }
};

#define I2C_1_ISR           isr_i2c2_er

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */


/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { GPIO_PIN(PORT_B, 3), 2 },
    { GPIO_PIN(PORT_B, 4), 3 },
    { GPIO_UNDEF, 14 }, /* VBAT */
};

#define VBAT_ADC            ADC_LINE(2) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
