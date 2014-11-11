/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_samr21-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Atmel SAM R21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF        (2U)
#define TIMER_0_EN         1
#define TIMER_1_EN         1

/* Timer 0 configuration */
#define TIMER_0_DEV        TC3->COUNT16
#define TIMER_0_CHANNELS   2
#define TIMER_0_MAX_VALUE  (0xffff)
#define TIMER_0_ISR        isr_tc3

/* Timer 1 configuration */
#define TIMER_1_DEV        TC4->COUNT32
#define TIMER_1_CHANNELS   2
#define TIMER_1_MAX_VALUE  (0xffffffff)
#define TIMER_1_ISR        isr_tc4

/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          SERCOM0->USART
#define UART_0_IRQ          SERCOM0_IRQn
#define UART_0_ISR          isr_sercom0
/* UART 0 pin configuration */
#define UART_0_PORT         (PORT->Group[0])
#define UART_0_TX_PIN       PIN_PA04
#define UART_0_RX_PIN       PIN_PA05
#define UART_0_PINS         (PORT_PA04 | PORT_PA05)
#define UART_0_REF_F        (8000000UL)


/* UART 1 device configuration */
#define UART_1_DEV
#define UART_1_IRQ
#define UART_1_ISR
/* UART 1 pin configuration */
#define UART_1_PORT
#define UART_1_PINS
/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF          (1U)
#define I2C_0_EN            1
#define I2C_1_EN            0
#define I2C_2_EN            0
#define I2C_3_EN            0
#define I2C_IRQ_PRIO        1

#define I2C_0_DEV           SERCOM3->I2CM
#define I2C_0_IRQ           SERCOM3_IRQn
#define I2C_0_ISR           isr_sercom3
/* I2C 0 pin configuration */
#define I2C_0_PORT          (PORT->Group[0])
#define I2C_SDA             PIN_PA16
#define I2C_SCL             PIN_PA17
#define I2C_0_PINS          (PORT_PA16 | PORT_PA17)
/* Default Clock Source on reset OSC8M - 8MHz */
#define I2C_0_REF_F         (8000000UL)

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (0U)
/** @} */

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          (4U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1

/* GPIO channel 0 config */
#define GPIO_0_DEV          PORT->Group[0]
#define GPIO_0_PIN          PIN_PA13
#define GPIO_0_EXTINT       13
/* GPIO channel 1 config */
/* SW0 Button, configure w/ GPIO_PULLUP and GPIO_FALLING */
#define GPIO_1_DEV          PORT->Group[0]
#define GPIO_1_PIN          PIN_PA28
#define GPIO_1_EXTINT       8
/* GPIO channel 2 config */
#define GPIO_2_DEV          PORT->Group[0]
#define GPIO_2_PIN          PIN_PA15
#define GPIO_2_EXTINT       15
/* GPIO channel 3 config */
#define GPIO_3_DEV          PORT->Group[0]
#define GPIO_3_PIN          PIN_PA19
#define GPIO_3_EXTINT       3
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
