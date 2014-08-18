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
#define TIMER_0_PRESCALER   (timerPrescale8)
#define TIMER_0_MAX_VALUE   (0xffffffff)
//#define TIMER_0_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define TIMER_0_ISR         TIMER0_IRQHandler
//#define TIMER_0_IRQ_CHAN    TIM2_IRQn
#define TIMER_0_IRQ_PRIO    1

/* Timer 0 configuration */
#define TIMER_1_DEV         TIMER1
#define TIMER_1_CHANNELS    3
#define TIMER_1_PRESCALER   (timerPrescale8)
#define TIMER_1_MAX_VALUE   (0xffffffff)
//#define TIMER_1_CLKEN()     (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
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
#define UART_0_DEV          USART0
#define UART_0_CLKEN()      CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART0
#define UART_0_RX_IRQ       USART0_RX_IRQn
#define UART_0_TX_IRQ       USART0_TX_IRQn
#define UART_0_RX_ISR       UART0_RX_IRQHandler
#define UART_0_TX_ISR       UART0_TX_IRQHandler
/* UART 0 pin configuration */
#define UART_0_PORT         gpioPortC
#define UART_0_PORT_CLKEN() CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO
#define UART_0_RX_PIN       1
#define UART_0_TX_PIN       0
#define UART_0_ROUTE        USART_ROUTE_LOCATION_LOC5


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
#define SPI_1_DEV          USART0
#define SPI_1_CLKEN()      CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART0
/* SPIE 0 pin configuration */
#define SPI_1_PORT         gpioPortC
#define SPI_1_PORT_CLKEN() CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO
#define SPI_1_CLK_PIN       1
#define SPI_1_CS_PIN        1
#define SPI_1_MOSI_PIN      1
#define SPI_1_MISO_PIN      0
#define SPI_1_ROUTE        USART_ROUTE_LOCATION_LOC5

#endif /* __PERIPH_CONF_H */
