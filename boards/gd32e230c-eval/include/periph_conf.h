/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_gd32e230c-eval
 * @{
 *
 * @file
 * @brief       Configuration of CPU peripherals for GD32E230C-EVAL board
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

// #include <stdint.h>

#include "macros/units.h"
#include "periph_cpu.h"
#include "periph_common_conf.h"

#include "cfg_i2c_default.h"
#include "cfg_spi_default.h"
#include "cfg_timer_default.h"
#include "cfg_uart_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .pin = GPIO_PIN(PORT_A, 0), .chan = 0 },
    { .pin = GPIO_PIN(PORT_A, 1), .chan = 1 },
    { .pin = GPIO_PIN(PORT_A, 2), .chan = 2 },
    { .pin = GPIO_PIN(PORT_A, 3), .chan = 3 },
    { .pin = GPIO_PIN(PORT_A, 4), .chan = 4 },
    { .pin = GPIO_PIN(PORT_A, 5), .chan = 5 },
    { .pin = GPIO_PIN(PORT_A, 6), .chan = 6 },
    { .pin = GPIO_PIN(PORT_A, 7), .chan = 7 },
    { .pin = GPIO_PIN(PORT_B, 0), .chan = 8 },
    { .pin = GPIO_PIN(PORT_B, 1), .chan = 9 },
    { .pin = GPIO_UNDEF, .chan = 16 },
    { .pin = GPIO_UNDEF, .chan = 17 },
    { .pin = GPIO_UNDEF, .chan = 18 }, // VBAT VREF ?
};

#define VBAT_ADC            ADC_LINE(6) /**< VBAT ADC line */
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = (TIMER_Type *) TIMER0,  // TODO for later pointers, the type is different b/c timer0 is "advanced"
        .rcu_mask = RCU_APB2EN_TIMER0EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  8), .cc_chan = 1 },
                        { .pin = GPIO_PIN(PORT_A,  9), .cc_chan = 2 },
                        { .pin = GPIO_PIN(PORT_A, 10), .cc_chan = 3 },
                        { .pin = GPIO_PIN(PORT_A, 11), .cc_chan = 4 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER2,
        .rcu_mask = RCU_APB1EN_TIMER2EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  6), .cc_chan = 1 }, //B4
                        { .pin = GPIO_PIN(PORT_A,  7), .cc_chan = 2 }, //B5
                        { .pin = GPIO_PIN(PORT_B,  0), .cc_chan = 3 },
                        { .pin = GPIO_PIN(PORT_B,  1), .cc_chan = 4 },
                    },
        // .af       = GPIO_AF_1,
        .bus      = APB1
    },
    {
        .dev      = (TIMER_Type *) TIMER13,
        .rcu_mask = RCU_APB1EN_TIMER13EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  1), .cc_chan = 1 }, //AF4 = A4, A7
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_0,
        .bus      = APB1
    },
    {
        .dev      = (TIMER_Type *) TIMER14,
        .rcu_mask = RCU_APB2EN_TIMER14EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_A,  2), .cc_chan = 1 }, //AF1 B14
                        { .pin = GPIO_PIN(PORT_A,  3), .cc_chan = 2 }, //AF1 B15
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_0,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER15,
        .rcu_mask = RCU_APB2EN_TIMER15EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  8), .cc_chan = 1 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
    {
        .dev      = (TIMER_Type *) TIMER16,
        .rcu_mask = RCU_APB2EN_TIMER16EN_Msk,
        .chan     = {
                        { .pin = GPIO_PIN(PORT_B,  9), .cc_chan = 1 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                        { .pin = GPIO_UNDEF,          .cc_chan = 0 },
                    },
        // .af       = GPIO_AF_2,
        .bus      = APB2
    },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
