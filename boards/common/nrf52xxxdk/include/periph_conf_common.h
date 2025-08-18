/*
 * SPDX-FileCopyrightText: 2016-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    /* Beware: Keep pwm_params.h in sync with the definitions here */
    { NRF_PWM0, {
        /* configure LED0 as PWM */
#ifdef LED0_PIN
        LED0_PIN,
#else
        GPIO_UNDEF,
#endif
        /* configure LED1 as PWM */
#ifdef LED1_PIN
        LED1_PIN,
#else
        GPIO_UNDEF,
#endif
        /* configure LED2 as PWM */
#ifdef LED2_PIN
        LED2_PIN,
#else
        GPIO_UNDEF,
#endif
        /* configure LED3 as PWM */
#ifdef LED3_PIN
        LED3_PIN,
#else
        GPIO_UNDEF,
#endif
    } },
};
#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
