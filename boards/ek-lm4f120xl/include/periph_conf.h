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
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_IRQ_PRIO       1
#define UART_CLK            ROM_SysCtlClockGet()  /* UART clock runs with 40MHz */
/* UART 0 device configuration */
#define UART_0_DEV          UART0_BASE
#define UART_0_CLK          (40000000)
#define UART_0_IRQ_CHAN     UART0_IRQn
#define UART_0_ISR          isr_uart0
/* UART 0 pin configuration */
#define UART_0_PORT         GPIOA
#define UART_0_TX_PIN       UART_PA1_U0TX
#define UART_0_RX_PIN       UART_PA0_U0RX

/* UART 1 device configuration */
#define UART_1_DEV          UART1_BASE
#define UART_1_CLK          (40000000)
#define UART_1_IRQ_CHAN     UART1_IRQn
#define UART_1_ISR          isr_uart1
/** @} */

/**
 * @brief   ADC configuration
 * @{
 */
#define ADC_NUMOF           (12)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
