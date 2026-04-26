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

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* No SPI, I2C, or PWM yet */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev = TIM2,
        .max = 0xffffffff,              /* 32-bit timer */
        .rcc_mask = RCC_APB1ENR1_TIM2EN,
        .bus = APB1,
        .irqn = TIM2_IRQn,
    }
};

#define TIMER_NUMOF        ARRAY_SIZE(timer_config)

#define TIMER_0_ISR        isr_tim2
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
