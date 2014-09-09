/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_stm32f0discovery
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the STM32F0discovery board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#include "em_device.h"
#include "em_timer.h"
#include "em_usart.h"
#include "em_cmu.h"

/******     From cortex-m3_common/cpu.h for RIOT compatibility      ******/
/**
 * @brief Macro has to be called in the beginning of each ISR
 */
#define ISR_ENTER()     asm("push {LR}")

/**
 * @brief Macro has to be called on each exit of an ISR
 */
#define ISR_EXIT()      asm("pop {r0} \n bx r0")
/******     End     ******/



/**
 * @name Clock system configuration
 * @{
 */
#define CLOCK_CORECLOCK     (24000000U)         /* desired core clock frequency */

/**
 * @name Timer configuration
 * @{
 */
#define TIMER_NUMOF         (2U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TIMER0
#define TIMER_0_CHANNELS    3
#define TIMER_0_PRESCALER   (timerPrescale32)
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_CLKEN()     CMU_ClockEnable(cmuClock_TIMER0, true)
#define TIMER_0_ISR         TIMER0_IRQHandler
//#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1

/* Timer 0 configuration */
#define TIMER_1_DEV         TIMER1
#define TIMER_1_CHANNELS    3
#define TIMER_1_PRESCALER   (timerPrescale32)
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_CLKEN()     CMU_ClockEnable(cmuClock_TIMER1, true)
#define TIMER_1_ISR         TIMER1_IRQHandler
//#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_1_IRQ_PRIO    1

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_CLKEN()      CMU_ClockEnable(cmuClock_UART0, true)
#define UART_0_RX_IRQ       UART0_RX_IRQn
#define UART_0_TX_IRQ       UART0_TX_IRQn
#define UART_0_RX_ISR       UART0_RX_IRQHandler
#define UART_0_TX_ISR       UART0_TX_IRQHandler
/* UART 0 pin configuration */
#define UART_0_PORT         gpioPortE
#define UART_0_PORT_CLKEN() CMU_ClockEnable(cmuClock_GPIO, true)
#define UART_0_RX_PIN       1
#define UART_0_TX_PIN       0
#define UART_0_ROUTE        USART_ROUTE_LOCATION_LOC1


/**
 * @name ADC configuration
 * @{
 */
#define ADC_NUMOF           (0U)
#define ADC_0_EN            0
#define ADC_MAX_CHANNELS    0



/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (0U)                                                        /* TODO */
#define PWM_0_EN            0
#define PWM_1_EN            0

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)                                                        /* TODO */
#define SPI_0_EN            0
#define SPI_1_EN            1

/* SPI 0 device configuration */
#define SPI_1_DEV           USART1
#define SPI_1_CLKEN()       CMU_ClockEnable(cmuClock_USART1, true);
/* SPIE 0 pin configuration */
#define SPI_1_PORT          gpioPortD
#define SPI_1_PORT_CLKEN()  CMU_ClockEnable(cmuClock_GPIO, true)
#define SPI_1_MOSI_PIN      0
#define SPI_1_MISO_PIN      1
#define SPI_1_CLK_PIN       2
#define SPI_1_CS_PIN        3
#define SPI_1_ROUTE         USART_ROUTE_LOCATION_LOC1

/**
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          (5U)
#define GPIO_0_EN           1
#define GPIO_1_EN           1
#define GPIO_2_EN           1
#define GPIO_3_EN           1
#define GPIO_4_EN           1
#define GPIO_IRQ_PRIO       1

/* IRQ config */
#define GPIO_IRQ_0          GPIO_0_PIN
#define GPIO_IRQ_1          GPIO_1_PIN
#define GPIO_IRQ_2          GPIO_2_PIN
#define GPIO_IRQ_3          GPIO_3_PIN
#define GPIO_IRQ_4          GPIO_4_PIN
#define GPIO_IRQ_5          GPIO_5_PIN

/* GPIO channel 0 config */
#define GPIO_0_PORT         gpioPortB   /* User button 0 */
#define GPIO_0_PIN          9

/* GPIO channel 1 config */
#define GPIO_1_PORT         gpioPortB   /* User button 1 */
#define GPIO_1_PIN          10

/* GPIO channel 2 config */
#define GPIO_2_PORT         gpioPortB
#define GPIO_2_PIN          11

/* GPIO channel 3 config */
#define GPIO_3_PORT         gpioPortB
#define GPIO_3_PIN          12

///* GPIO channel 4 config */
#define GPIO_4_PORT         gpioPortD
#define GPIO_4_PIN          15

#endif /* __PERIPH_CONF_H */
