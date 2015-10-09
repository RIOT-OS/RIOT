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
 * @brief Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1
#define TIMER_IRQ_PRIO      1

/* Timer 0 configuration */
#define TIMER_0_DEV         LPC_CT32B1
#define TIMER_0_CHANNELS    4
#define TIMER_0_PRESCALER   (48U)
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
 *  * @name GPIO configuration
 *   * @{
 */
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
#define GPIO_16_EN          1
#define GPIO_17_EN          1
#define GPIO_18_EN          1
#define GPIO_19_EN          1
#define GPIO_20_EN          1
#define GPIO_21_EN          1
#define GPIO_22_EN          1
#define GPIO_23_EN          1
#define GPIO_24_EN          1
#define GPIO_25_EN          1
#define GPIO_26_EN          1
#define GPIO_27_EN          1
#define GPIO_28_EN          1
#define GPIO_29_EN          1
#define GPIO_30_EN          1
#define GPIO_31_EN          1

/* WeIO Pin 0 : LPC GPIO0_18 */
#define GPIO_0_PORT         0
#define GPIO_0_PIN          18
/* WeIO Pin 1 : LPC GPIO0_19 */
#define GPIO_1_PORT         0
#define GPIO_1_PIN          19
/* WeIO Pin 2 : LPC GPIO0_9 */
#define GPIO_2_PORT         0
#define GPIO_2_PIN          9
/* WeIO Pin 3 : LPC GPIO0_8 */
#define GPIO_3_PORT         0
#define GPIO_3_PIN          8
/* WeIO Pin 4 : LPC GPIO0_10 */
#define GPIO_4_PORT         0
#define GPIO_4_PIN          10
/* WeIO Pin 5 : LPC GPIO0_2 */
#define GPIO_5_PORT         0
#define GPIO_5_PIN          2
/* WeIO Pin 6 : LPC GPIO0_7 */
#define GPIO_6_PORT         0
#define GPIO_6_PIN          7
/* WeIO Pin 7 : LPC GPIO0_17 */
#define GPIO_7_PORT         0
#define GPIO_7_PIN          17

/* WeIO Pin 8 : LPC GPIO0_21 */
#define GPIO_8_PORT         0
#define GPIO_8_PIN          21
/* WeIO Pin 9 : LPC GPIO1_21 */
#define GPIO_9_PORT         1
#define GPIO_9_PIN          21
/* WeIO Pin 10 : LPC GPIO1_20 */
#define GPIO_10_PORT        1
#define GPIO_10_PIN         20
/* WeIO Pin 11 : LPC GPIO0_20 */
#define GPIO_11_PORT        0
#define GPIO_11_PIN         20
/* WeIO Pin 12 : LPC GPIO1_16 */
#define GPIO_12_PORT        1
#define GPIO_12_PIN         16
/* WeIO Pin 13 : LPC GPIO1_19 */
#define GPIO_13_PORT        1
#define GPIO_13_PIN         19
/* WeIO Pin 14 : LPC GPIO1_22 */
#define GPIO_14_PORT        1
#define GPIO_14_PIN         22
/* WeIO Pin 15 : LPC GPIO1_23 */
#define GPIO_15_PORT        1
#define GPIO_15_PIN         23

/* WeIO Pin 16 : LPC GPIO1_27 */
#define GPIO_16_PORT        1
#define GPIO_16_PIN         27
/* WeIO Pin 17 : LPC GPIO1_28 */
#define GPIO_17_PORT        1
#define GPIO_17_PIN         28
/* WeIO Pin 18 : LPC GPIO1_13 */
#define GPIO_18_PORT        1
#define GPIO_18_PIN         13
/* WeIO Pin 19 : LPC GPIO1_14 */
#define GPIO_19_PORT        1
#define GPIO_19_PIN         14
/* WeIO Pin 20 : LPC GPIO1_15 */
#define GPIO_20_PORT        1
#define GPIO_20_PIN         15
/* WeIO Pin 21 : LPC GPIO1_24 */
#define GPIO_21_PORT        1
#define GPIO_21_PIN         24
/* WeIO Pin 22 : LPC GPIO1_25 */
#define GPIO_22_PORT        1
#define GPIO_22_PIN         25
/* WeIO Pin 23 : LPC GPIO1_26 */
#define GPIO_23_PORT        1
#define GPIO_23_PIN         26

/* WeIO Pin 24 : LPC GPIO0_11 */
#define GPIO_24_PORT        0
#define GPIO_24_PIN         11
/* WeIO Pin 25 : LPC GPIO0_12 */
#define GPIO_25_PORT        0
#define GPIO_25_PIN         12
/* WeIO Pin 26 : LPC GPIO0_13 */
#define GPIO_26_PORT        0
#define GPIO_26_PIN         13
/* WeIO Pin 27 : LPC GPIO0_14 */
#define GPIO_27_PORT        0
#define GPIO_27_PIN         14
/* WeIO Pin 28 : LPC GPIO0_15 */
#define GPIO_28_PORT        0
#define GPIO_28_PIN         15
/* WeIO Pin 29 : LPC GPIO0_16 */
#define GPIO_29_PORT        0
#define GPIO_29_PIN         16
/* WeIO Pin 30 : LPC GPIO0_22 */
#define GPIO_30_PORT        0
#define GPIO_30_PIN         22
/* WeIO Pin 31 : LPC GPIO0_23 */
#define GPIO_31_PORT        0
#define GPIO_31_PIN         23
/** @} */

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
