/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_nucleo-u385rg
 * @{
 *
 * @file
 * @brief       Minimal peripheral configuration for STM32U385 (bring-up)
 * 
 * @author      Adarsh Nair Mullachery
 */

/* Nucleo board provides 32.768 kHz LSE for RTC (same as other Nucleo-U boards) */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE 1
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
 * @name I2C configuration
 * @{
 * @note Nucleo Arduino connector D14/D15 = PB9/PB8 (SDA/SCL). I2C1 on APB1,
 *       AF4 per RM0487 / alternate-function tables (same convention as other
 *       STM32 Nucleo boards). Kernel clock: RCC CCIPR1 I2C1SEL — leave @c 0 to
 *       keep reset default (typically PCLK1); OR @ref RCC_CCIPR1_I2C1SEL if a
 *       different RM0487 setting is required.
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
                  { .pin = GPIO_UNDEF, .cc_chan = 0 },
                  { .pin = GPIO_UNDEF, .cc_chan = 0 },
                  { .pin = GPIO_UNDEF, .cc_chan = 0 } },
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
    }
};

#define UART_0_ISR isr_usart1
#define UART_NUMOF (1)
/** @} */

/**
 * @name    USB device (USB DRD FS) configuration
 * @note    D+/D- on ST morpho: PA12 / PA11, alternate function as per
 *          the STM32U385 and Nucleo documentation.
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
        .af         = GPIO_AF10,
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
