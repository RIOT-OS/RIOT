/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_pca10000
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nRF51822 board pca10000
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock configuration
 *
 * @note: the radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_CORECLOCK     (16000000U)     /* fixed for all NRF51822 */
#define CLOCK_CRYSTAL       (16U)           /* set to  0: internal RC oscillator
                                                      16: 16MHz crystal
                                                      32: 32MHz crystal */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_1_EN          0
#define TIMER_2_EN          0
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         NRF_TIMER0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffffff)
#define TIMER_0_BITMODE     TIMER_BITMODE_BITMODE_24Bit
#define TIMER_0_ISR         isr_timer0
#define TIMER_0_IRQ         TIMER0_IRQn

/* Timer 1 configuration */
#define TIMER_1_DEV         NRF_TIMER1
#define TIMER_1_CHANNELS    3
#define TIMER_1_MAX_VALUE   (0xffff)
#define TIMER_1_BITMODE     TIMER_BITMODE_BITMODE_16Bit
#define TIMER_1_ISR         isr_timer1
#define TIMER_1_IRQ         TIMER1_IRQn

/* Timer 2 configuration */
#define TIMER_2_DEV         NRF_TIMER2
#define TIMER_2_CHANNELS    3
#define TIMER_2_MAX_VALUE   (0xffff)
#define TIMER_2_BITMODE     TIMER_BITMODE_BITMODE_16Bit
#define TIMER_2_ISR         isr_timer2
#define TIMER_2_IRQ         TIMER2_IRQn
/** @} */

/**
 * @name Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ_PRIO        1

#define RTT_DEV             NRF_RTC1
#define RTT_IRQ             RTC1_IRQn
#define RTT_ISR             isr_rtc1
#define RTT_MAX_VALUE       (0xffffff)
#define RTT_FREQUENCY       (10)            /* in Hz */
#define RTT_PRESCALER       (3275U)         /* run with 10 Hz */
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART pin configuration */
#define UART_HWFLOWCTRL   1
#define UART_PIN_RX       11
#define UART_PIN_TX       9
#define UART_PIN_RTS      8
#define UART_PIN_CTS      10
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (1U)
/** @} */

/**
 * @name Radio device configuration
 *
 * The radio is not guarded by a NUMOF define, as the radio is selected by its
 * own module in the build system.
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
