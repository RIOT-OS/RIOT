/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-u575zi-q
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-u575zi-q board
 *
 * @author      Nils Ollrogge <nils.ollrogge@mailbox.tu-dresden.de>
 */

/* Add specific clock configuration (HSE, LSE) for this board here */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE 1
#endif

#include "cfg_timer_tim5.h"
#include "cfg_usb_otg_fs_u5.h"
#include "clk_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin = GPIO_PIN(PORT_A, 10),
        .tx_pin = GPIO_PIN(PORT_A, 9),
        .rx_af = GPIO_AF7,
        .tx_af = GPIO_AF7,
        .bus = APB2,
        .irqn = USART1_IRQn,
        .type = STM32_USART,
        .clk_src = 0, /* Use APB clock */
    },
    {
        .dev = LPUART1,
        .rcc_mask = RCC_APB3ENR_LPUART1EN,
        .rx_pin = GPIO_PIN(PORT_G, 8),
        .tx_pin = GPIO_PIN(PORT_G, 7),
        .rx_af = GPIO_AF8,
        .tx_af = GPIO_AF8,
        .bus = APB3,
        .irqn = LPUART1_IRQn,
        .type = STM32_LPUART,
        .clk_src = 0, /* Use APB clock */
    },
};

#define UART_0_ISR (isr_usart1)
#define UART_1_ISR (isr_lpuart1)

#define UART_NUMOF ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),    /* Arduino D11 */
        .miso_pin = GPIO_PIN(PORT_A, 6),    /* Arduino D12 */
        .sclk_pin = GPIO_PIN(PORT_A, 5),    /* Arduino D13 */
        .cs_pin = GPIO_UNDEF,
        .mosi_af = GPIO_AF5,
        .miso_af = GPIO_AF5,
        .sclk_af = GPIO_AF5,
        .cs_af = GPIO_AF5,
        .rccmask = RCC_APB2ENR_SPI1EN,
        .apbbus = APB2,
    },
};

#define SPI_NUMOF ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
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
        .rcc_mask = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask = RCC_CCIPR1_I2C1SEL_1,
        .irqn = I2C1_ER_IRQn,
    },
    {
        .dev = I2C2,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_F, 1),
        .sda_pin = GPIO_PIN(PORT_F, 0),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR1_I2C2EN,
        .rcc_sw_mask = RCC_CCIPR1_I2C2SEL_1,
        .irqn = I2C2_ER_IRQn,
    },
};

#define I2C_0_ISR isr_i2c1_er
#define I2C_1_ISR isr_i2c2_er
#define I2C_NUMOF ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * To find appriopate device and channel find in the MCU datasheet table
 * concerning "Alternate function AF0 to AF7" a text similar to TIM[X]_CH[Y],
 * where:
 * TIM[X] - is device,
 * [Y] - describes used channel (indexed from 0), for example TIM2_CH1 is
 * channel 0 in configuration structure (cc_chan - field),
 * Port column in the table describes connected port.
 *
 * For Nucleo-U575ZI-Q this information is in the datasheet, Table 27, page 127.
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    { .dev = TIM2,
      .rcc_mask = RCC_APB1ENR1_TIM2EN,
      .chan = { { .pin = GPIO_PIN(PORT_A, 0) /* CN10 D32 */, .cc_chan = 0 },
          { .pin = GPIO_PIN(PORT_A, 1) /* CN10  A8 */, .cc_chan = 1 },
          { .pin = GPIO_PIN(PORT_A, 2) /*  CN9  A1 */, .cc_chan = 2 },
          { .pin = GPIO_PIN(PORT_A, 3) /*  CN9  A0 */, .cc_chan = 3 } },
      .af = GPIO_AF1,
      .bus = APB1 },
    { .dev = TIM3,
      .rcc_mask = RCC_APB1ENR1_TIM3EN,
      .chan = { { .pin = GPIO_PIN(PORT_B, 4) /*  CN7 D25 */, .cc_chan = 0 },
          { .pin = GPIO_PIN(PORT_B, 5) /*  CN7 D22 */, .cc_chan = 1 },
          { .pin = GPIO_PIN(PORT_B, 0) /*  CN9  A3 */, .cc_chan = 2 },
          { .pin = GPIO_PIN(PORT_B, 1) /* CN10  A6 */, .cc_chan = 3 } },
      .af = GPIO_AF2,
      .bus = APB1 },
    { .dev = TIM4,
      .rcc_mask = RCC_APB1ENR1_TIM4EN,
      .chan = { { .pin = GPIO_PIN(PORT_D, 12) /*  CN7 D19 */, .cc_chan = 0 },
          { .pin = GPIO_PIN(PORT_B, 7) /* Blue LD2 */, .cc_chan = 1 },
          { .pin = GPIO_PIN(PORT_D, 14) /*  CN7 D10 */, .cc_chan = 2 },
          { .pin = GPIO_PIN(PORT_D, 15) /*  CN7  D9 */, .cc_chan = 3 } },
      .af = GPIO_AF2,
      .bus = APB1 },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
