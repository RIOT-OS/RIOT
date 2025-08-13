/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_kw41z
 * @{
 *
 * @file
 * @name        Default I2C configuration for KW41Z based boards
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @name I2C configuration
* @{
*/
static const i2c_conf_t i2c_config[] = {
    {
        .i2c = I2C1,
        .scl_pin = GPIO_PIN(PORT_C, 2),
        .sda_pin = GPIO_PIN(PORT_C, 3),
        .freq = CLOCK_CORECLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C1_IRQn,
        .scl_pcr = (PORT_PCR_MUX(3)),
        .sda_pcr = (PORT_PCR_MUX(3)),
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           (isr_i2c1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
