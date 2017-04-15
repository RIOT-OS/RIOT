/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_seeduino_arch-pro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Seeeduino Archo Pro board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         LPC_TIM0
#define TIMER_0_CHANNELS    4
#define TIMER_0_FREQ        (96000000ul)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     (LPC_SC->PCONP |= (1 << 1))
#define TIMER_0_CLKDIS()    (LPC_SC->PCONP &= ~(1 << 1))
#define TIMER_0_PLKSEL()    (LPC_SC->PCLKSEL0 |= (1 << 2))
#define TIMER_0_ISR         isr_timer0
#define TIMER_0_IRQ         TIMER0_IRQn
/** @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (2U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          LPC_UART0
#define UART_0_CLKSEL()     (LPC_SC->PCLKSEL0 &= ~(0x3 << 6))       /* PCLK := CCLK / 4 */
#define UART_0_CLKEN()      (LPC_SC->PCONP |= (1 << 3))
#define UART_0_CLKDIS()     (LPC_SC->PCONP &= ~(1 << 3))
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_TX_PINSEL    (LPC_PINCON->PINSEL0)
#define UART_0_RX_PINSEL    (LPC_PINCON->PINSEL0)
#define UART_0_TX_PINMODE   (LPC_PINCON->PINMODE0)
#define UART_0_RX_PINMODE   (LPC_PINCON->PINMODE0)
#define UART_0_TX_PIN       (3)
#define UART_0_RX_PIN       (2)
#define UART_0_AF           (1)

/* UART 1 device configuration */
#define UART_1_DEV          LPC_UART3
#define UART_1_CLKSEL()     (LPC_SC->PCLKSEL1 &= ~(0x3 << 18))      /* PCLK := CCLK / 4 */
#define UART_1_CLKEN()      (LPC_SC->PCONP |= (1 << 25))
#define UART_1_CLKDIS()     (LPC_SC->PCONP &= ~(1 << 25))
#define UART_1_IRQ          UART3_IRQn
#define UART_1_ISR          isr_uart3
/* UART 1 pin configuration */
#define UART_1_TX_PINSEL    (LPC_PINCON->PINSEL0)
#define UART_1_RX_PINSEL    (LPC_PINCON->PINSEL0)
#define UART_1_TX_PINMODE   (LPC_PINCON->PINMODE0)
#define UART_1_RX_PINMODE   (LPC_PINCON->PINMODE0)
#define UART_1_RX_PIN       (0)
#define UART_1_TX_PIN       (1)
#define UART_1_AF           (2)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
