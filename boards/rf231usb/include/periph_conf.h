/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_rf231usb
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the rf231usb
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_NUMOF         (1U)
#define TIMER_0_EN          1

/* Timer 0 configuration */
#define TIMER_0_DEV         TC0
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffffffff)
#define TIMER_0_ISR1        isr_tc0
#define TIMER_0_ISR2        isr_tc1

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_IRQ_PRIO       1

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

/* UART 0 device configuration */
#define UART_0_DEV          UART0
#define UART_0_IRQ          UART0_IRQn
#define UART_0_ISR          isr_uart0
#define UART_0_PORT         PIOA
#define UART_0_PINS         (PIO_PA9 | PIO_PA10)

/* SPI 0 device config */
#define SPI_0_DEV           SPI
#define SPI_0_CLKEN()       (PMC->PMC_PCER0 |= (1 << ID_SPI));
#define SPI_0_CLKDIS()      (PMC->PMC_PCER0 &= ~(1 << ID_SPI));
#define SPI_0_IRQ           SPI_IRQn
#define SPI_0_IRQ_HANDLER   isr_spi
#define SPI_0_IRQ_PRIO      1

/* SPI 0 pin configuration */
#define SPI_0_MISO_PIN      PIO_PA12A_MISO
#define SPI_0_MOSI_PIN      PIO_PA13A_MOSI
#define SPI_0_SCK_PIN       PIO_PA14A_SPCK
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
#ifndef GPIO_IRQ_PRIO
#define GPIO_IRQ_PRIO       CPU_DEFAULT_IRQ_PRIO
#endif

#define PIOA_EN             (1)
#define PIOB_EN             (1)
#define PIOC_EN             (0)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
