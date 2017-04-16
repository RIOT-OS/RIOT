/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_microbit
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the BBC micro:bit
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
 * @name    Clock configuration
 *
 * @note    The radio will not work with the internal RC oscillator!
 *
 * @{
 */
#define CLOCK_CORECLOCK     (16000000U)     /* fixed for all nRF51822 */
#define CLOCK_HFCLK         (16U)           /* set to  0: internal RC oscillator
                                                      16: 16MHz crystal
                                                      32: 32MHz crystal */
#define CLOCK_LFCLK         (0)             /* set to  0: internal RC oscillator
                                             *         1: 32.768 kHz crystal
                                             *         2: derived from HFCLK */
/** @} */

/**
 * @name Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0,
        .channels =  3,
        .bitmode  = TIMER_BITMODE_BITMODE_24Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_16Bit,
        .irqn     = TIMER1_IRQn
    },
    {
        .dev      = NRF_TIMER2,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_16Bit,
        .irqn     = TIMER2_IRQn
    }
};

#define TIMER_0_ISR         isr_timer0
#define TIMER_1_ISR         isr_timer1
#define TIMER_2_ISR         isr_timer2

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
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
/* UART pin configuration */
#define UART_HWFLOWCTRL     0
#define UART_PIN_RX         25
#define UART_PIN_TX         24
/** @} */

/**
 * @name   I2C (TWI) configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev     = NRF_TWI0,
        .pin_scl = 0,
        .pin_sda = 30,
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
#define ADC_NUMOF          (0)
/** @} */

/**
 * @name   Radio device configuration
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
/** @} */
