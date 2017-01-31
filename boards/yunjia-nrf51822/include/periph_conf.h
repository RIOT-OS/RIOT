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

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 19,
        .mosi = 17,
        .miso = 18
    },
    {
        .dev  = NRF_SPI1,
        .sclk = 22,
        .mosi = 20,
        .miso = 21
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */

/**
 * @name   I2C (TWI) configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev     = NRF_TWI0,
        .pin_scl = 23,
        .pin_sda = 24,
        .ppi     = 0
    }
};

#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name   ADC configuration
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

#endif /* PERIPH_CONF_H */
