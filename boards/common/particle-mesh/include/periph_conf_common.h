/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_particle-mesh
 * @{
 *
 * @file
 * @brief       Common peripheral configuration for the Particle Mesh
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_i2c_default.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPIM0,
        .sclk = GPIO_PIN(1, 15),
        .mosi = GPIO_PIN(1, 13),
        .miso = GPIO_PIN(1, 14),
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    PWM configuration
 *
 * A single PWM device is used to map the three channels of the on-board RGB
 * LED
 *
 * @{
 */

static const pwm_conf_t pwm_config[] = {
    { NRF_PWM0, { LED0_PIN, LED1_PIN, LED2_PIN, GPIO_UNDEF } }
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
