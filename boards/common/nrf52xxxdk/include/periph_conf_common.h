/*
 * Copyright (C) 2016-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nrf52xxxdk NRF52 DK common
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"
#include "cfg_clock_32_1.h"
#include "cfg_i2c_default.h"
#include "cfg_rtt_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    { NRF_PWM0, { 28, 29, 30, 31 } }
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @brief Enable the internal DC/DC converter
 */
#define NRF5X_ENABLE_DCDC

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
