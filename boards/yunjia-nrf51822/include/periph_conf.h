/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_yunjia-nrf51822
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Yunjia NRF51822 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock configuration
 *
 * @note: the radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_CORECLOCK     (16000000U)     /* fixed for all NRF51822 */
#define CLOCK_CRYSTAL       (16U)           /* set to  0: internal RC oscillator
                                                      16: 16MHz crystal
                                                      32: 32MHz crystal */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* dev, channels, width */
    { NRF_TIMER0, 3, TIMER_BITMODE_BITMODE_24Bit, TIMER0_IRQn }
};

#define TIMER_0_ISR         isr_timer0

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
/* UART pin configuration */
#define UART_PIN_RX         1
#define UART_PIN_TX         2
/** @} */

/**
 * @name Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ_PRIO        1

#define RTT_DEV             NRF_RTC1
#define RTT_IRQ             RTC1_IRQn
#define RTT_ISR             isr_rtc1
#define RTT_MAX_VALUE       (0xffffff)
#define RTT_FREQUENCY       (10)            /* in Hz */
#define RTT_PRESCALER       (3275U)         /* run with 10 Hz */
/** @} */

/**
 * @name SPI configuration
 * @{
 */
#define SPI_NUMOF           (2U)
#define SPI_0_EN            1
#define SPI_1_EN            1
#define SPI_IRQ_PRIO        1

/* SPI_0 device configuration */
#define SPI_0_DEV           NRF_SPI0
#define SPI_0_PIN_MOSI      17
#define SPI_0_PIN_MISO      18
#define SPI_0_PIN_SCK       19

/* SPI_1 device configuration */
#define SPI_1_DEV           NRF_SPI1
#define SPI_1_PIN_MOSI      20
#define SPI_1_PIN_MISO      21
#define SPI_1_PIN_SCK       22
/** @} */

/**
 * @brief   ADC configuration
 *
 * The configuration consists simply of a list of channels that should be used
 * @{
 */
#define ADC_CONFIG          {4, 5, 6, 7}
#define ADC_NUMOF           (4)
/** @} */

/**
 * @name Radio device configuration
 *
 * The radio is not guarded by a NUMOF define, as the radio is selected by its
 * own module in the build system.
 * @{
 */
#define RADIO_IRQ_PRIO      1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
