/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_CORECLOCK     (64000000U)     /* fixed for all NRF52832 */
#define CLOCK_CRYSTAL       (32U)           /* set to  0: internal RC oscillator
                                                      32: 32MHz crystal */
/** @} */

/**
 * @brief Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         NRF_TIMER0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_BITMODE     TIMER_BITMODE_BITMODE_32Bit
#define TIMER_0_ISR         isr_timer0
#define TIMER_0_IRQ         TIMER0_IRQn
/** @} */

/**
 * @brief Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             NRF_RTC0
#define RTT_IRQ             RTC0_IRQn
#define RTT_ISR             isr_rtc0
#define RTT_MAX_VALUE       (0xffffff)
#define RTT_FREQUENCY       (10)            /* in Hz */
#define RTT_PRESCALER       (3275U)         /* run with 10 Hz */
/** @} */

/**
 * @brief UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         8
#define UART_PIN_TX         6
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
