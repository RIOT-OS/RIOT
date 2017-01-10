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

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

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
 * @brief Timer configuration
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
 * @brief UART configuration
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
 * @brief SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1
/* @} */

/**
 * @brief PWM configuration
 * @{
 */
#define PWM_0_EN            1
#define PWM_0_CHANNELS      3
#define PWM_1_EN            1
#define PWM_1_CHANNELS      3
#define PWM_NUMOF           (2U)

/* PWM0 common configuration */
#define PWM_0_DEV           LPC_CT16B0
#define PWM_0_CLK           BIT7
/* PWM_0 channel configuration */
#define PWM_0_CH0_EN        1
#define PWM_0_CH0_IOCON     LPC_IOCON->PIO1_13
#define PWM_0_CH0_AF        0x82

#define PWM_0_CH1_EN        1
#define PWM_0_CH1_IOCON     LPC_IOCON->PIO1_14
#define PWM_0_CH1_AF        0x82

#define PWM_0_CH2_EN        1
#define PWM_0_CH2_IOCON     LPC_IOCON->PIO1_15
#define PWM_0_CH2_AF        0x82

/* PWM1 common configuration */
#define PWM_1_DEV           LPC_CT32B0
#define PWM_1_CLK           BIT9
/* PWM_1 channel configuration */

#define PWM_1_CH0_EN        1
#define PWM_1_CH0_IOCON     LPC_IOCON->PIO1_24
#define PWM_1_CH0_AF        0x81

#define PWM_1_CH1_EN        1
#define PWM_1_CH1_IOCON     LPC_IOCON->PIO1_25
#define PWM_1_CH1_AF        0x81

#define PWM_1_CH2_EN        1
#define PWM_1_CH2_IOCON     LPC_IOCON->PIO1_26
#define PWM_1_CH2_AF        0x81
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
