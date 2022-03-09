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
#include "cfg_timer_012.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
/* UART pin configuration */
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

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
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
