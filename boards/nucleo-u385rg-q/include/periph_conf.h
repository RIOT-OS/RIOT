/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-u385rg-q
 * @{
 *
 * @file
 * @brief       Minimal peripheral configuration for STM32U385 (bring-up)
 *
 * @author      Adarsh Nair Mullachery <adarsh.mullachery@tuhh.de>
 */

/* Nucleo board provides 32.768 kHz LSE for RTC (same as other Nucleo-U boards) */
#ifndef CONFIG_BOARD_HAS_LSE
#  define CONFIG_BOARD_HAS_LSE 1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   SPI configuration
 * @{
 * @note  Arduino D11/D12/D13 (MOSI/MISO/SCK) on Nucleo-U: PA7/PA6/PA5. SPI1 on
 *        APB2; clock gate @ref RCC_APB2ENR_SPI1EN in stm32u385xx.h. Alternate
 *        function AF5 for SPI1 on port A (RM0487 AF table).
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
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
 * @name    I2C configuration
 * @{
 * @note    I2C1 on PB6/PB7 (SCL/SDA), the Arduino connector pins D15/D14
 *          on this board (see UM3062). The I2C1 kernel clock is left at its
 *          reset default (PCLK1).
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = I2C1,
        .speed = I2C_SPEED_NORMAL,
        .scl_pin = GPIO_PIN(PORT_B, 6),
        .sda_pin = GPIO_PIN(PORT_B, 7),
        .scl_af = GPIO_AF4,
        .sda_af = GPIO_AF4,
        .bus = APB1,
        .rcc_mask = RCC_APB1ENR1_I2C1EN,
        .rcc_sw_mask = 0,
        .irqn = I2C1_ER_IRQn,
    },
};

#define I2C_0_ISR isr_i2c1_er
#define I2C_NUMOF ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    Timer configuration
 * @{
 * @note    TIM2 is reserved for @ref pwm_config (User LED / TIM2_CH1 on PA5).
 *          TIM3 is used here as the RIOT timer backend (general-purpose timer).
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM3,
        .max = 0xffffffff,
        .rcc_mask = RCC_APB1ENR1_TIM3EN,
        .bus = APB1,
        .irqn = TIM3_IRQn,
    }
};

#define TIMER_NUMOF        ARRAY_SIZE(timer_config)

#define TIMER_0_ISR        isr_tim3
/** @} */

/**
 * @name    PWM configuration
 *
 * TIM2 channel mapping follows the MCU alternate-function table (RM0487).
 * TIM2_CH1 on PA5 uses AF1 and connects to the green User LED (same pin as
 * Arduino D13 when SPI1 is not in use; see boards/common/nucleo64/doc.md).
 *
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev = TIM2,
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
        .chan = { { .pin = GPIO_PIN(PORT_A, 5) /* User LED */, .cc_chan = 0 },
                  { .pin = GPIO_PIN(PORT_B, 3),  .cc_chan = 1 }, /* CH2: Arduino D3 */
                  { .pin = GPIO_PIN(PORT_B, 10), .cc_chan = 2 }, /* CH3: Arduino D6 */
                  { .pin = GPIO_PIN(PORT_B, 11), .cc_chan = 3 }  /* CH4: Standard extension */
                },
        .af = GPIO_AF1,
        .bus = APB1,
    },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)

/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        /* ST-Link Virtual COM Port (STDIO, UART_DEV(0)) — USART1 on PA9/PA10 */
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF7,
        .tx_af      = GPIO_AF7,
        .bus        = APB2,
        .irqn       = USART1_IRQn,
        .type       = STM32_USART,
        .clk_src    = 0, /* Use APB clock */
    },
    {
        /* LPUART1 on PA2/PA3 */
        .dev        = LPUART1,
        .rcc_mask   = RCC_APB3ENR_LPUART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF8,
        .bus        = APB3,
        .irqn       = LPUART1_IRQn,
        .type       = STM32_LPUART,
        .clk_src    = 0,
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_lpuart1)
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    ADC configuration
 * @{
 * @note Arduino CN8 (A0–A5) pinout for NUCLEO-U385RG-Q per ST UM3062 Table 14:
 *       A0=PA0/IN5, A1=PA1/IN6, A2=PA4/IN9, A3=PB0/IN15, A4=PC1/IN2, A5=PC0/IN1.
 *       All lines use ADC1 (@c dev 0). PC0/PC1 are also Arduino I2C3 on the
 *       connector; this board’s default I2C is I2C1 on PB8/PB9.
 */
/**
 * @brief   Internal ADC1 channel for VREFINT on STM32U3 (channel 0).
 */
#ifndef VREFINT_ADC_CHAN
#  define VREFINT_ADC_CHAN  0
#endif

/* ADC1 channel numbers below are verified on hardware (potentiometer sweep on
 * each Arduino analog pin). The STM32U385 ADC1 input mux maps
 *   PC0=IN1, PC1=IN2, PA0=IN3, PA1=IN4, PA2=IN5, PA3=IN6, PA4=IN7, PB0=IN13. */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .dev = 0, .chan = 3 },   /* A0  ADC1_IN3  */
    { .pin = GPIO_PIN(PORT_A, 1), .dev = 0, .chan = 4 },   /* A1  ADC1_IN4  */
    { .pin = GPIO_PIN(PORT_A, 4), .dev = 0, .chan = 7 },   /* A2  ADC1_IN7  */
    { .pin = GPIO_PIN(PORT_B, 0), .dev = 0, .chan = 13 },  /* A3  ADC1_IN13 */
    { .pin = GPIO_PIN(PORT_C, 1), .dev = 0, .chan = 2 },   /* A4  ADC1_IN2  */
    { .pin = GPIO_PIN(PORT_C, 0), .dev = 0, .chan = 1 },   /* A5  ADC1_IN1  */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = 16 },           /* VBAT/4 internal (RM0487: VBAT is IN16) */
    { .pin = GPIO_UNDEF, .dev = 0, .chan = VREFINT_ADC_CHAN }, /* VREFINT (internal channel 0) */
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define VREFINT_ADC         ADC_LINE(7) /**< VREFINT ADC line (for vref_mv()) */

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name    USB device (USB DRD FS) configuration
 * @note    D+/D- on PA12/PA11 (user USB Type-C connector). The USB DRD FS
 *          PHY drives the pads directly, so no alternate function is used;
 *          the driver keeps the pins in analog mode.
 * @note    48MHz USB digital clock must be provided by the clock tree in
 *          @ref cpu_stm32 "stmclk" when using this peripheral.
 * @note    On STM32U3, USB is on APB2 and gated via @c RCC->APB2ENR
 *          (see RM0487, memory map + RCC section).
 * @{
 */
static const stm32_usbdev_fs_config_t stm32_usbdev_fs_config[] = {
    {
        .base_addr  = (uintptr_t)USB,
        .rcc_mask   = RCC_U3_USBDEV_FS_RMASK,
        .dm         = GPIO_PIN(PORT_A, 11),
        .dp         = GPIO_PIN(PORT_A, 12),
        .af         = GPIO_AF_UNDEF,
        .disconn    = GPIO_UNDEF,
        .irqn       = USB_FS_IRQn,
        .apb        = APB2,
    },
};

#define USBDEV_ISR          isr_usb_fs
#define USBDEV_NUMOF        ARRAY_SIZE(stm32_usbdev_fs_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
