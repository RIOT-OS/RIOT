/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-due
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Arduino Due board
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Andreas "Paul" Pauli <andreas.pauli@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          1
#define TIMER_2_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC0
#define TIMER_0_CHANNELS    6
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_ISR1        isr_tc0
#define TIMER_0_ISR2        isr_tc1

/* Timer 1 configuration */
#define TIMER_1_DEV         TC1
#define TIMER_1_CHANNELS    6
#define TIMER_1_MAX_VALUE   (0xffffffff)
#define TIMER_1_ISR1        isr_tc3
#define TIMER_1_ISR2        isr_tc4

/* Timer 2 configuration */
#define TIMER_2_DEV         TC2
#define TIMER_2_CHANNELS    6
#define TIMER_2_MAX_VALUE   (0xffffffff)
#define TIMER_2_ISR1        isr_tc6
#define TIMER_2_ISR2        isr_tc7
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (4U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_3_EN           1

/* UART 0 device configuration */
#define UART_0_DEV          UART
#define UART_0_CLKEN()      (PMC->PMC_PCER0 |= (1 << ID_UART))
#define UART_0_CLKDIS()     (PMC->PMC_PCER0 &= ~(1 << ID_UART))
#define UART_0_IRQ          UART_IRQn
#define UART_0_ISR          isr_uart
/* UART 0 pin configuration */
#define UART_0_PORT         PIOA
#define UART_0_PINS         (PIO_PA8 | PIO_PA9)

/* UART 1 device configuration */
#define UART_1_DEV          USART0
#define UART_1_CLKEN()      (PMC->PMC_PCER0 |= (1 << ID_USART0))
#define UART_1_CLKDIS()     (PMC->PMC_PCER0 &= ~(1 << ID_USART0))
#define UART_1_IRQ          USART0_IRQn
#define UART_1_ISR          isr_usart0
/* UART 1 pin configuration */
#define UART_1_PORT         PIOA
#define UART_1_PINS         (PIO_PA10 | PIO_PA11)

/* UART 1 device configuration */
#define UART_2_DEV          USART1
#define UART_2_CLKEN()      (PMC->PMC_PCER0 |= (1 << ID_USART1))
#define UART_2_CLKDIS()     (PMC->PMC_PCER0 &= ~(1 << ID_USART1))
#define UART_2_IRQ          USART1_IRQn
#define UART_2_ISR          isr_usart1
/* UART 1 pin configuration */
#define UART_2_PORT         PIOA
#define UART_2_PINS         (PIO_PA12 | PIO_PA13)

/* UART 1 device configuration */
#define UART_3_DEV          USART3
#define UART_3_CLKEN()      (PMC->PMC_PCER0 |= (1 << ID_USART3))
#define UART_3_CLKDIS()     (PMC->PMC_PCER0 &= ~(1 << ID_USART3))
#define UART_3_IRQ          USART3_IRQn
#define UART_3_ISR          isr_usart3
/* UART 1 pin configuration */
#define UART_3_PORT         PIOD
#define UART_3_PINS         (PIO_PD4 | PIO_PD5)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define RANDOM_NUMOF        (1U)
/** @} */

/**
* @name SPI configuration
* @{
*/
#define SPI_NUMOF           (1U)
#define SPI_0_EN            1

/* SPI 0 device config */
#define SPI_0_DEV           SPI0
#define SPI_0_CLKEN()       (PMC->PMC_PCER0 |= (1 << ID_SPI0));
#define SPI_0_CLKDIS()      (PMC->PMC_PCER0 &= ~(1 << ID_SPI0));
#define SPI_0_IRQ           SPI0_IRQn
#define SPI_0_IRQ_HANDLER   isr_spi0
#define SPI_0_IRQ_PRIO      1

/* SPI 0 pin configuration */
#define SPI_0_MISO_PIN      PIO_PA25A_SPI0_MISO
#define SPI_0_MOSI_PIN      PIO_PA26A_SPI0_MOSI
#define SPI_0_SCK_PIN       PIO_PA27A_SPI0_SPCK
#define SPI_0_MISO_PORT     PIOA
#define SPI_0_MOSI_PORT     PIOA
#define SPI_0_SCK_PORT      PIOA
#define SPI_0_MISO_PORT_CLKEN()  (PMC->PMC_PCER0 |= (1 << ID_PIOA));
#define SPI_0_MOSI_PORT_CLKEN()  (PMC->PMC_PCER0 |= (1 << ID_PIOA));
#define SPI_0_SCK_PORT_CLKEN()   (PMC->PMC_PCER0 |= (1 << ID_PIOA));
/** @} */

/**
 * @name PWM configuration
 * @{
 */
#define PWM_NUMOF           (1U)
#define PWM_0_EN            (1)
#define PWM_MAX_VALUE       (0xffff)
#define PWM_MAX_CHANNELS    (4U)

/* PWM_0 configuration */
#define PWM_0_DEV           PWM
#define PWM_0_PID           ID_PWM
#define PWM_0_CHANNELS      (4U)
#define PWM_0_DEV_CH0       (&(PWM_0_DEV->PWM_CH_NUM[4]))
#define PWM_0_ENA_CH0       PWM_ENA_CHID4
#define PWM_0_PORT_CH0      PIOC
#define PWM_0_PIN_CH0       PIO_PC21B_PWML4
#define PWM_0_DEV_CH1       (&(PWM_0_DEV->PWM_CH_NUM[5]))
#define PWM_0_ENA_CH1       PWM_ENA_CHID5
#define PWM_0_PORT_CH1      PIOC
#define PWM_0_PIN_CH1       PIO_PC22B_PWML5
#define PWM_0_DEV_CH2       (&(PWM_0_DEV->PWM_CH_NUM[6]))
#define PWM_0_ENA_CH2       PWM_ENA_CHID6
#define PWM_0_PORT_CH2      PIOC
#define PWM_0_PIN_CH2       PIO_PC23B_PWML6
#define PWM_0_DEV_CH3       (&(PWM_0_DEV->PWM_CH_NUM[7]))
#define PWM_0_ENA_CH3       PWM_ENA_CHID7
#define PWM_0_PORT_CH3      PIOC
#define PWM_0_PIN_CH3       PIO_PC24B_PWML7
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
/** @} */
