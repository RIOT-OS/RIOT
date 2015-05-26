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
 * @name GPIO configuration
 * @{
 */

/* GPIO pin mapping */
enum {
    PA2,    /* EXT1 P10, SW0 */
    PB10,   /* orange LED */
    PB6,    /* EXT1 P05 */
    PA12,   /* EXT1 P07 */
    PB4,    /* EXT1 P09 */
    PA5,    /* EXT1 P15 SPI_SS */
    PA6,    /* EXT1 P16 SPI_MOSI */
    PA4,    /* EXT1 P17 SPI_MISO */
    PA7,    /* EXT1 P18 SPI_SCK */
    GPIO_UNUSED,
    PB5,
    PA3,
    PB7,
    PB9,
    PA13,
};

/* define this to the value of GPIO_UNNUSED.
 * Unfortunately the preprocessor can't access the enum's value.*/
#define GPIO_NUMOF 9 /* same as GPIO_UNUSED */

enum {
    EXT1_SPI, /* EXT1 -> SPI0 */
};

enum {
    EXT1_P07       = PA12,
    EXT1_P09       = PB4,
    EXT1_P10       = PA2,
    EXT1_SPI_SS    = PA5,
    EXT1_SPI_MOSI  = PA6,
    EXT1_SPI_MISO  = PA4,
    EXT1_SPI_SCK   = PA7,
};

#define GPIO_EXTI0_EN 0
#define GPIO_EXTI1_EN 0
#define GPIO_EXTI2_EN 1 /* for PA02/BUTTON0 */
#define GPIO_EXTI3_EN 0
#define GPIO_EXTI4_EN 1 /* for PB04 */
#define GPIO_EXTI5_EN 0
#define GPIO_EXTI6_EN 0
#define GPIO_EXTI7_EN 0
#define GPIO_EXTI8_EN 0
#define GPIO_EXTI9_EN 0
#define GPIO_EXTI10_EN 0
#define GPIO_EXTI11_EN 0
#define GPIO_EXTI12_EN 1
#define GPIO_EXTI13_EN 0
#define GPIO_EXTI14_EN 0
#define GPIO_EXTI15_EN 0

/* defines to satisfy periph/gpio.h
 * In order to support GPIO_0..n in applications, you have to define these here.
 */
#define GPIO_0_EN 1
#define GPIO_1_EN 1
#define GPIO_2_EN 1
#define GPIO_3_EN 1
#define GPIO_4_EN 1
#define GPIO_5_EN 1
#define GPIO_6_EN 1
#define GPIO_7_EN 1
#define GPIO_8_EN 1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
