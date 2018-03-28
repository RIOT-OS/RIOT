/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_ek-lm4f120xl
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the ek-lm4f120xl board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Define the nominal CPU core clock in this board
 * @{
 */
#define CLK80                           1
#define CLK50                           2
#define CLK40                           3
#define CLK16                           4
#define CLK1                            5
#define CLOCK_SOURCE                    CLK40
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration
 *
 * WTIMER0 is a 32/64bits timer.
 * We use timer_a as TIMER_0
 */
#define TIMER_0_CHANNELS    1
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_ISR         isr_wtimer0a
#define TIMER_0_IRQ_CHAN    Timer0A_IRQn

/* Timer 1 configuration
 *
 * WTIMER1 is a 32/64bits timer.
 * We use timer_a as TIMER_1
 */

#define TIMER_1_CHANNELS    1
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR         isr_wtimer1a
#define TIMER_1_IRQ_CHAN    Timer1A_IRQn
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev                = 0,
        .uart_txint_mode    = UART_TXINT_MODE_EOT,
        .uart_fifo_tx       = UART_FIFO_TX4_8,
        .uart_fifo_rx       = UART_FIFO_RX4_8,
        .uart_irq_chan      = UART0_IRQn,
        .gpio_sysctl        = SYSCTL_PERIPH_GPIOA,
        .gpio_port          = GPIO_PORTA_BASE,
        .pins = {
            .rx             = GPIO_PA0_U0RX,
            .tx             = GPIO_PA1_U0TX,
            .mask_rx        = (GPIO_PIN_0),
            .mask_tx        = (GPIO_PIN_1),
        }
    },
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_uart0

/* macros common across all UARTs */
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))

/**
 * @name   ADC configuration
 * @{
 */
#define ADC_NUMOF           (12)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_confs[] = {
    {
        .ssi_sysctl = SYSCTL_PERIPH_SSI0,
        .ssi_base = SSI0_BASE,
        .gpio_sysctl = SYSCTL_PERIPH_GPIOA,
        .gpio_port = GPIO_PORTA_BASE,
        .pins = {
            .clk = GPIO_PA2_SSI0CLK,
            .fss = GPIO_PA3_SSI0FSS,
            .rx = GPIO_PA4_SSI0RX,
            .tx = GPIO_PA5_SSI0TX,
            .mask = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
        }
    },
    {
        .ssi_sysctl = SYSCTL_PERIPH_SSI1,
        .ssi_base = SSI1_BASE,
        .gpio_sysctl = SYSCTL_PERIPH_GPIOF,
        .gpio_port = GPIO_PORTF_BASE,
        .pins = {
            .clk = GPIO_PF2_SSI1CLK,
            .fss = GPIO_PF3_SSI1FSS,
            .rx = GPIO_PF0_SSI1RX,
            .tx = GPIO_PF1_SSI1TX,
            .mask = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
        }
    },
    {
        .ssi_sysctl = SYSCTL_PERIPH_SSI2,
        .ssi_base = SSI2_BASE,
        .gpio_sysctl = SYSCTL_PERIPH_GPIOB,
        .gpio_port = GPIO_PORTB_BASE,
        .pins = {
            .clk = GPIO_PB4_SSI2CLK,
            .fss = GPIO_PB5_SSI2FSS,
            .rx = GPIO_PB6_SSI2RX,
            .tx = GPIO_PB7_SSI2TX,
            .mask = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7
        }
    },
    {
        .ssi_sysctl = SYSCTL_PERIPH_SSI3,
        .ssi_base = SSI3_BASE,
        .gpio_sysctl = SYSCTL_PERIPH_GPIOD,
        .gpio_port = GPIO_PORTD_BASE,
        .pins = {
            .clk = GPIO_PD0_SSI3CLK,
            .fss = GPIO_PD1_SSI3FSS,
            .rx = GPIO_PD2_SSI3RX,
            .tx = GPIO_PD3_SSI3TX,
            .mask = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
        }
    },
};

#define SPI_NUMOF (sizeof(spi_confs) / sizeof(spi_confs[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
