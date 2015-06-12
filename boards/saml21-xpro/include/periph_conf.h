/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *               2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_saml21-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Atmel SAM L21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @autor       Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GCLK reference speed
 */
#define GCLK_REF (16000000U)

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF        (1U)
#define TIMER_0_EN         1

/* Timer 0 configuration */
#define TIMER_0_DEV        TC0->COUNT32
#define TIMER_0_CHANNELS   1
#define TIMER_0_MAX_VALUE  (0xffffffff)
#define TIMER_0_ISR        isr_tc0
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          SERCOM3->USART
#define UART_0_IRQ          SERCOM3_IRQn
#define UART_0_ISR          isr_sercom3
/* UART 0 pin configuration */
#define UART_0_PORT         (PORT->Group[0])
#define UART_0_TX_PIN       (22)
#define UART_0_RX_PIN       (23)
#define UART_0_PINS         (((PORT_PA22 | PORT_PA23) >> 16) | PORT_WRCONFIG_HWSEL)
#define UART_0_REF_F        (16000000UL)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF          (1)
#define SPI_0_EN           1
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (0)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF       (0U)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
#define RTC_NUMOF           (0)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_FREQUENCY       (32768U)
#define RTT_MAX_VALUE        (0xffffffffU)
#define RTT_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
