/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_atsamr21-xpro
 * @{
 *
 * @file        periph_conf.h
 * @brief       Peripheral MCU configuration for the Atmel SAM R21 Xplained Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TCC0
#define TIMER_0_CHANNELS    6
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_ISR1        isr_tcc0
// #define TIMER_0_ISR2        isr_tc1

/* Timer 1 configuration */
#define TIMER_1_DEV         TCC1
#define TIMER_1_CHANNELS    6
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR1        isr_tcc1
// #define TIMER_1_ISR2        isr_tc4

/* Timer 2 configuration */
#define TIMER_2_DEV         TCC2
#define TIMER_2_CHANNELS    6
#define TIMER_2_MAX_VALUE   (0xffffffff)
#define TIMER_2_ISR1        isr_tcc2
// #define TIMER_2_ISR2        isr_tc7
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
#define UART_0_DEV          Sercom->USART
#define UART_0_IRQ          SERCOM0_IRQn
#define UART_0_ISR          isr_sercom0
/* UART 0 pin configuration */
#define UART_0_PORT         Port->Group[0]
#define UART_0_PINS         (PIN_PA08 | PIN_PA09)

/* UART 1 device configuration */
#define UART_1_DEV
#define UART_1_IRQ
#define UART_1_ISR
/* UART 1 pin configuration */
#define UART_1_PORT
#define UART_1_PINS
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
#define GPIO_NUMOF          (3U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           0
#define GPIO_4_EN           0
#define GPIO_5_EN           0
#define GPIO_6_EN           0
#define GPIO_7_EN           0
#define GPIO_8_EN           0
#define GPIO_9_EN           0
#define GPIO_10_EN          0
#define GPIO_11_EN          0
#define GPIO_12_EN          0
#define GPIO_13_EN          0
#define GPIO_14_EN          0
#define GPIO_15_EN          0

//These GPIO ports are chosen as to not conflict with other functionallity
//May have to implement more
/* GPIO channel 0 config */
#define GPIO_0_PIN          PIN_PA13 //EXT1 PIN5
/* GPIO channel 1 config */
#define GPIO_1_PIN          PIN_PA28 //EXT1 PIN6, and USER_BUTTON
/* GPIO channel 2 config */
#define GPIO_2_PIN          PIN_PA15 //EXT3 PIN5
/* GPIO channel 3 config */
#define GPIO_3_PIN          
/* GPIO channel 4 config */
#define GPIO_4_PIN          
/* GPIO channel 5 config */
#define GPIO_5_PIN          
/* GPIO channel 6 config */
#define GPIO_6_PIN          
/* GPIO channel 7 config */
#define GPIO_7_PIN          
/* GPIO channel 8 config */        
#define GPIO_8_PIN          
/* GPIO channel 9 config */ 
#define GPIO_9_PIN          
/* GPIO channel 10 config */ 
#define GPIO_10_PIN         
/* GPIO channel 11 config */
#define GPIO_11_PIN         
/* GPIO channel 12 config */
#define GPIO_12_PIN         
/* GPIO channel 13 config */
#define GPIO_13_PIN         
/* GPIO channel 14 config */
#define GPIO_14_PIN         
/* GPIO channel 15 config */
#define GPIO_15_PIN         
/** @} */

#endif /* __PERIPH_CONF_H */
/** @} */
