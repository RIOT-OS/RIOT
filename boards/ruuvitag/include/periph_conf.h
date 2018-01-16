/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ruuvitag
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the RuuviTag
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
 * @name    Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_HFCLK         (32U)           /* set to  0: internal RC oscillator
                                             *        32: 32MHz crystal */
#define CLOCK_LFCLK         (1)             /* set to  0: internal RC oscillator
                                             *         1: 32.768 kHz crystal
                                             *         2: derived from HFCLK */
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER1,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER1_IRQn
    }
};

#define TIMER_0_ISR         isr_timer1

#define TIMER_NUMOF         (sizeof(timer_config) / sizeof(timer_config[0]))
/** @} */

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             (1)             /* NRF_RTC1 */
#define RTT_MAX_VALUE       (0x00ffffff)
#define RTT_FREQUENCY       (1024)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 29,
        .mosi = 25,
        .miso = 28,
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_PIN_RX         GPIO_PIN(0, 4)
#define UART_PIN_TX         GPIO_PIN(0, 5)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
