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
#include "macros/units.h"

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
#define CLOCK_CORECLOCK                 MHZ(80)
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = WTIMER0_BASE,
        .max      = 0xffffffff,
        .irqn     = Timer0A_IRQn,
        .sysctl   = SYSCTL_PERIPH_WTIMER0,
        .intbase  = INT_WTIMER0A,
        .channels = 1
    },
    {
        .dev      = WTIMER1_BASE,
        .max      = 0xffffffff,
        .irqn     = Timer1A_IRQn,
        .sysctl   = SYSCTL_PERIPH_WTIMER1,
        .intbase  = INT_WTIMER1A,
        .channels = 1
    },
};

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)

#define TIMER_0_ISR         isr_wtimer0a
#define TIMER_1_ISR         isr_wtimer1a
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_IRQ_PRIO       1
/* UART clock runs with 40MHz */
#define UART_CLK            ROM_SysCtlClockGet()
/* UART 0 device configuration */
#define UART_0_DEV          UART0_BASE
#define UART_0_CLK          (40000000)
#define UART_0_IRQ_CHAN     UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_TX_PIN       UART_PA1_U0TX
#define UART_0_RX_PIN       UART_PA0_U0RX
/** @} */

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

#define SPI_NUMOF ARRAY_SIZE(spi_confs)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
