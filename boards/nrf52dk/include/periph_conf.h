/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_CORECLOCK     (64000000U)     /* fixed for all NRF52832 */
#define CLOCK_CRYSTAL       (32U)           /* set to  0: internal RC oscillator
                                                      32: 32MHz crystal */
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    /* dev, channels, width, IRQn */
    { NRF_TIMER1, 3, TIMER_BITMODE_BITMODE_32Bit, TIMER1_IRQn }
};

#define TIMER_0_ISR         isr_timer1

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @brief Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             NRF_RTC1
#define RTT_IRQ             RTC1_IRQn
#define RTT_ISR             isr_rtc1
#define RTT_MAX_VALUE       (0xffffff)
#define RTT_FREQUENCY       (10)            /* in Hz */
#define RTT_PRESCALER       (3275U)         /* run with 10 Hz */
/** @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         8
#define UART_PIN_TX         6
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 15,
        .mosi = 13,
        .miso = 14 }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
