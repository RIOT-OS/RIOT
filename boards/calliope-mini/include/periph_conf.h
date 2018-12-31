/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_calliope-mini
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Calliope mini
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_16_0.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Timer configuration
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
    }
};

#define TIMER_0_ISR         isr_timer0
#define TIMER_1_ISR         isr_timer1

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
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
/* UART pin configuration */
#define UART_HWFLOWCTRL     0
#define UART_PIN_RX         25
#define UART_PIN_TX         24
/** @} */

/**
 * @name    I2C (TWI) configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev     = NRF_TWI0,
        .pin_scl = 19,
        .pin_sda = 20,
        .ppi     = 0,
        .speed   = I2C_SPEED_NORMAL,
    }
};

#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
#define PWM_NUMOF           (1U)
#define PWM_TIMER           NRF_TIMER2
#define PWM_PIN             (0U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
