/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_weio
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the WeIO board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (48000000U)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         LPC_CT32B1
#define TIMER_0_CHANNELS    4
#define TIMER_0_FREQ        (48000000ul)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10))
#define TIMER_0_CLKDIS()    (LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 10))
#define TIMER_0_ISR         isr_ct32b1
#define TIMER_0_IRQ         TIMER_32_1_IRQn
/* @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       2

/* UART 0 device configuration */
#define UART_0_DEV          LPC_USART
#define UART_0_CLKSEL()     (LPC_SYSCON->UARTCLKDIV = (1))       /* PCLK := CCLK / 1 */
#define UART_0_CLKEN()      (LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12))
#define UART_0_CLKDIS()     (LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 12))
#define UART_0_IRQ          UART_IRQn
#define UART_0_ISR          isr_usart0
/* UART 0 pin configuration */
#define UART_0_TX_PINSEL    (LPC_IOCON->PIO0_19)
#define UART_0_RX_PINSEL    (LPC_IOCON->PIO0_18)
#define UART_0_TX_PINMODE   (LPC_IOCON->PIO0_19)
#define UART_0_RX_PINMODE   (LPC_IOCON->PIO0_18)
#define UART_0_TX_PIN       (1)
#define UART_0_RX_PIN       (2)
#define UART_0_AF           (1)
/* @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev        = LPC_SSP0,
        .preset_bit = (1 << 0),
        .ahb_bit    = (1 << 11)
    },
    {
        .dev        = LPC_SSP1,
        .preset_bit = (1 << 2),
        .ahb_bit    = (1 << 18)
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/* @} */

/**
 * @brief PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev     = LPC_CT16B0,
        .pins    = {
            &LPC_IOCON->PIO1_13,
            &LPC_IOCON->PIO1_14,
            &LPC_IOCON->PIO1_15
        },
        .clk_bit = BIT7,
        .af      = 0x02
    },
    {
        .dev     = LPC_CT32B0,
        .pins    = {
            &LPC_IOCON->PIO1_24,
            &LPC_IOCON->PIO1_25,
            &LPC_IOCON->PIO1_26
        },
        .clk_bit = BIT9,
        .af      = 0x01
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
