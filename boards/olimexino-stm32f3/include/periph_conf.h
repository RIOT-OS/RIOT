/*
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_olimexino-stm32f3
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the OLIMEXINO STM32F3 board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE      1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#  define CONFIG_BOARD_HAS_HSE      1
#endif

/* Configure clock to run at 8 MHz / 1 * 9 = 72 MHz, which is the maximum
 * freq and compatible with USB */
#if !defined(CONFIG_CLOCK_PLL_PREDIV) && !defined(CONFIG_CLOCK_PLL_MUL)
#  define CONFIG_CLOCK_PLL_PREDIV   1
#  define CONFIG_CLOCK_PLL_MUL      9
#endif

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
        { GPIO_PIN(PORT_C, 0), 0, 10 },
        { GPIO_PIN(PORT_C, 1), 0, 11 },
        { GPIO_PIN(PORT_C, 2), 0, 12 },
        { GPIO_PIN(PORT_C, 3), 0, 13 },
        { GPIO_PIN(PORT_C, 4), 0, 14 },
        { GPIO_PIN(PORT_C, 5), 0, 15 }
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM15,
        .rcc_mask = RCC_APB2ENR_TIM15EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 3), .cc_chan = 1 },     /* D0 */
                      { .pin = GPIO_PIN(PORT_A, 2), .cc_chan = 0 } },   /* D1 */
        .af       = GPIO_AF9,
        .bus      = APB2
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 6), .cc_chan = 0 },     /* D6 */
                      { .pin = GPIO_PIN(PORT_B, 7), .cc_chan = 1 } },   /* D9 */
        .af       = GPIO_AF2,
        .bus      = APB1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM2,
        .max = UINT32_MAX,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .bus = APB1,
        .irqn = TIM2_IRQn
    },
    {
        .dev = TIM1,
        .max = UINT16_MAX,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .bus = APB2,
        .irqn = TIM1_CC_IRQn
    },
    {
        .dev = TIM3,
        .max = UINT16_MAX,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .bus = APB1,
        .irqn = TIM3_IRQn
    },
    {
        .dev = TIM8,
        .max = UINT16_MAX,
        .rcc_mask = RCC_APB2ENR_TIM8EN,
        .bus = APB2,
        .irqn = TIM8_CC_IRQn
    },
};

#define TIMER_0_ISR         isr_tim2
#define TIMER_1_ISR         isr_tim1_cc
#define TIMER_2_ISR         isr_tim3
#define TIMER_3_ISR         isr_tim8_cc

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin = GPIO_PIN(PORT_A, 3),
        .tx_pin = GPIO_PIN(PORT_A, 2),
        .bus = APB1,
        .irqn = USART2_IRQn
    },
    {
        .dev = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin = GPIO_PIN(PORT_A, 10),
        .tx_pin = GPIO_PIN(PORT_A, 9),
        .bus = APB2,
        .irqn = USART1_IRQn
    },
    {
        .dev = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin = GPIO_PIN(PORT_B, 11),
        .tx_pin = GPIO_PIN(PORT_B, 10),
        .bus = APB1,
        .irqn = USART3_IRQn
    }
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    Real time counter configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#  define RTT_FREQUENCY     (32768)      /* in Hz */
#endif
/** @} */

/**
 * @name    I2C configuration
 * @note    This board may require external pullup resistors for i2c operation.
 * @{
 */

static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_A, 9),
        .sda_pin        = GPIO_PIN(PORT_A, 10),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .rcc_sw_mask    = RCC_CFGR3_I2C2SW,
        .irqn           = I2C2_ER_IRQn
    },
};

#define I2C_0_ISR           isr_i2c2_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_PIN(PORT_A, 4),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),
        .miso_pin = GPIO_PIN(PORT_B, 14),
        .sclk_pin = GPIO_PIN(PORT_B, 13),
        .cs_pin   = GPIO_PIN(PORT_B, 12),
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    USB configuration
 * @{
 */
/**
 * @brief USB device FS configuration
 */
static const stm32_usbdev_fs_config_t stm32_usbdev_fs_config[] = {
    {
        .base_addr  = (uintptr_t)USB,
        .rcc_mask   = RCC_APB1ENR_USBEN,
        .irqn       = USB_LP_CAN_RX0_IRQn,
        .apb        = APB1,
        .dm         = GPIO_PIN(PORT_A, 11),
        .dp         = GPIO_PIN(PORT_A, 12),
        .af         = GPIO_AF14,
        .disconn    = GPIO_PIN(PORT_C, 12),
        .flags      = STM32_USBDEV_FS_CONFIG_FLAG_DISCONN_INVERTED,
    },
};

/**
 * @brief Interrupt function name mapping
 */
#define USBDEV_ISR              isr_usb_lp_can_rx0

/**
 * @brief Number of available USB device FS peripherals
 */
#define USBDEV_NUMOF            ARRAY_SIZE(stm32_usbdev_fs_config)
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
