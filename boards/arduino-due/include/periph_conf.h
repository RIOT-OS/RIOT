/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     board_arduino-due
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Arduino Due board
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
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
#define UART_IRQ_PRIO       1

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
 * @name GPIO configuration
 * @{
 */
#define GPIO_NUMOF          (16U)
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

/* GPIO channel 0 config */
#define GPIO_0_DEV          PIOA
#define GPIO_0_PIN          PIO_PA14
#define GPIO_0_IRQ          PIOA_IRQn
#define GPIO_A14_MAP        0
/* GPIO channel 1 config */
#define GPIO_1_DEV          PIOD
#define GPIO_1_PIN          PIO_PD0
#define GPIO_1_IRQ          PIOD_IRQn
#define GPIO_D0_MAP         1
/* GPIO channel 2 config */
#define GPIO_2_DEV          PIOD
#define GPIO_2_PIN          PIO_PD2
#define GPIO_2_IRQ          PIOD_IRQn
#define GPIO_D2_MAP         2
/* GPIO channel 3 config */
#define GPIO_3_DEV          PIOD
#define GPIO_3_PIN          PIO_PD6
#define GPIO_3_IRQ          PIOD_IRQn
#define GPIO_D6_MAP         3
/* GPIO channel 4 config */
#define GPIO_4_DEV          PIOA
#define GPIO_4_PIN          PIO_PA7
#define GPIO_4_IRQ          PIOA_IRQn
#define GPIO_A7_MAP         4
/* GPIO channel 5 config */
#define GPIO_5_DEV          PIOC
#define GPIO_5_PIN          PIO_PC1
#define GPIO_5_IRQ          PIOC_IRQn
#define GPIO_C1_MAP         5
/* GPIO channel 6 config */
#define GPIO_6_DEV          PIOC
#define GPIO_6_PIN          PIO_PC3
#define GPIO_6_IRQ          PIOC_IRQn
#define GPIO_C3_MAP         6
/* GPIO channel 7 config */
#define GPIO_7_DEV          PIOC
#define GPIO_7_PIN          PIO_PC5
#define GPIO_7_IRQ          PIOC_IRQn
#define GPIO_C5_MAP         7
/* GPIO channel 8 config */
#define GPIO_8_DEV          PIOC
#define GPIO_8_PIN          PIO_PC7
#define GPIO_8_IRQ          PIOC_IRQn
#define GPIO_C7_MAP         8
/* GPIO channel 9 config */
#define GPIO_9_DEV          PIOC
#define GPIO_9_PIN          PIO_PC9
#define GPIO_9_IRQ          PIOC_IRQn
#define GPIO_C9_MAP         9
/* GPIO channel 10 config */
#define GPIO_10_DEV         PIOA
#define GPIO_10_PIN         PIO_PA20
#define GPIO_10_IRQ         PIOA_IRQn
#define GPIO_A20_MAP        10
/* GPIO channel 11 config */
#define GPIO_11_DEV         PIOC
#define GPIO_11_PIN         PIO_PC18
#define GPIO_11_IRQ         PIOC_IRQn
#define GPIO_C18_MAP        11
/* GPIO channel 12 config */
#define GPIO_12_DEV         PIOC
#define GPIO_12_PIN         PIO_PC16
#define GPIO_12_IRQ         PIOC_IRQn
#define GPIO_C16_MAP        12
/* GPIO channel 13 config */
#define GPIO_13_DEV         PIOC
#define GPIO_13_PIN         PIO_PC14
#define GPIO_13_IRQ         PIOC_IRQn
#define GPIO_C14_MAP        13
/* GPIO channel 14 config */
#define GPIO_14_DEV         PIOC
#define GPIO_14_PIN         PIO_PC12
#define GPIO_14_IRQ         PIOC_IRQn
#define GPIO_C12_MAP        14
/* GPIO channel 15 config */
#define GPIO_15_DEV         PIOB
#define GPIO_15_PIN         PIO_PB14
#define GPIO_15_IRQ         PIOB_IRQn
#define GPIO_B14_MAP        15
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
/** @} */
