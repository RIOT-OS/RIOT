/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_pca10000
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nRF51822 board pca10000
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

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
#define TIEMR_1_BITMODE     TIMER_BITMODE_BITMODE_16Bit
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

#define RTT_DEV             NRF_RTC0
#define RTT_IRQ             RTC0_IRQn
#define RTT_ISR             isr_rtc0
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
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          16
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_5_EN           1
#define GPIO_6_EN           1
#define GPIO_7_EN           1
#define GPIO_8_EN           1
#define GPIO_9_EN           1
#define GPIO_10_EN          1
#define GPIO_11_EN          1
#define GPIO_12_EN          1
#define GPIO_13_EN          1
#define GPIO_14_EN          1
#define GPIO_15_EN          1
#define GPIO_IRQ_PRIO       1

/* GPIO pin configuration */
#define GPIO_0_PIN          0
#define GPIO_1_PIN          1
#define GPIO_2_PIN          2
#define GPIO_3_PIN          3
#define GPIO_4_PIN          4
#define GPIO_5_PIN          5
#define GPIO_6_PIN          6
#define GPIO_7_PIN          7
#define GPIO_8_PIN          8
#define GPIO_9_PIN          9
#define GPIO_10_PIN         10
#define GPIO_11_PIN         11
#define GPIO_12_PIN         12
#define GPIO_13_PIN         13
#define GPIO_14_PIN         14
#define GPIO_15_PIN         15
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
