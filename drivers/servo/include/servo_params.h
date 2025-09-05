/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_servo
 *
 * @{
 * @file
 * @brief       Default configuration for servo devices
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "board.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "periph/timer.h"
#include "saul_reg.h"
#include "servo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default servo PWM configuration
 * @{
 */
#ifndef SERVO_PWM_PARAM_DEV
/**
 * @brief   PWM device to use to control the servo
 *
 * Defaults to `PWM_DEV(0)`.
 */
#define SERVO_PWM_PARAM_DEV             PWM_DEV(0)
#endif

#ifndef SERVO_PWM_PARAM_RES
/**
 * @brief   PWM resolution to use to control the servo
 *
 * Defaults to `UINT16_MAX`.
 */
#define SERVO_PWM_PARAM_RES             UINT16_MAX
#endif

#ifndef SERVO_PWM_PARAM_FREQ
/**
 * @brief   PWM frequency in Hertz to use to control the servo
 *
 * Defaults to `50UL` Hz.
 */
#define SERVO_PWM_PARAM_FREQ            50
#endif

#ifndef SERVO_PWM_PARAMS
/**
 * @brief   PWM parameters for controlling a servo
 */
#define SERVO_PWM_PARAMS    {\
        .pwm    = SERVO_PWM_PARAM_DEV, \
        .freq   = SERVO_PWM_PARAM_FREQ, \
        .res    = SERVO_PWM_PARAM_RES, \
    }
#endif
/** @} */

/**
 * @brief   Servo PWM parameters
 */
static const servo_pwm_params_t servo_pwm_params[] =
{
    SERVO_PWM_PARAMS
};

/**
 * @name    Default servo timer configuration
 * @{
 */
#ifndef SERVO_TIMER_PARAM_DEV
/**
 * @brief   Timer to use to control the servo
 *
 * Defaults to `TIMER_DEV(1)`.
 */
#define SERVO_TIMER_PARAM_DEV           TIMER_DEV(1)
#endif

#ifndef SERVO_TIMER_PARAM_TIMER_FREQ
/**
 * @brief   Timer frequency to use to control the servo in Hz
 *
 * Defaults to 1 MHz
 */
#define SERVO_TIMER_PARAM_TIMER_FREQ    MHZ(1)
#endif

#ifndef SERVO_TIMER_PARAM_SERVO_FREQ
/**
 * @brief   Servo frequency in Hertz
 *
 * Defaults to `50UL` Hz.
 */
#define SERVO_TIMER_PARAM_SERVO_FREQ    50
#endif

#ifndef SERVO_TIMER_PARAM_TIMER_CTX
/**
 * @brief   Default servo timer context
 *
 * Allocated in drivers/servo/timer.c
 */
extern servo_timer_ctx_t servo_timer_default_ctx;
/**
 * @brief   Memory needed for book keeping
 *
 * Defaults to `&servo_timer_default_ctx`. One context per timer used is needed.
 * E.g. when 4 servos are connected but all are controlled with the same timer
 * peripheral, only one context is needed.
 */
#define SERVO_TIMER_PARAM_TIMER_CTX     (&servo_timer_default_ctx)
#endif

#ifndef SERVO_TIMER_PARAMS
/**
 * @brief   TIMER parameters for controlling a servo
 */
#define SERVO_TIMER_PARAMS    {\
        .timer      = SERVO_TIMER_PARAM_DEV, \
        .timer_freq = SERVO_TIMER_PARAM_TIMER_FREQ, \
        .servo_freq = SERVO_TIMER_PARAM_SERVO_FREQ, \
        .ctx        = SERVO_TIMER_PARAM_TIMER_CTX, \
    }
#endif
/** @} */

/**
 * @brief   Servo timer parameters
 */
static const servo_timer_params_t servo_timer_params[] =
{
    SERVO_TIMER_PARAMS
};

/**
 * @name    Default servo configuration
 * @{
 */
#ifndef SERVO_PARAM_PWM_PARAMS
/**
 * @brief   PWM parameters
 *
 * Defaults to `&servo_pwm_params[0]`.
 */
#define SERVO_PARAM_PWM_PARAMS          (&servo_pwm_params[0])
#endif

#ifndef SERVO_PARAM_TIMER_PARAMS
/**
 * @brief   Timer parameters
 *
 * Defaults to `&servo_timer_params[0]`.
 */
#define SERVO_PARAM_TIMER_PARAMS        (&servo_timer_params[0])
#endif

#ifndef SERVO_PARAM_PWM_CHAN
/**
 * @brief   PWM channel to use to control the servo
 *
 * Defaults to `0`
 */
#define SERVO_PARAM_PWM_CHAN            0
#endif

#ifndef SERVO_PARAM_TIMER_CHAN
/**
 * @brief   Timer channel used to clear the servo pin
 *
 * Defaults to `1`
 */
#define SERVO_PARAM_TIMER_CHAN          1
#endif

#ifndef SERVO_PARAM_PIN
/**
 * @brief   GPIO pin the servo input is connected to
 *
 * @note    Only used with @ref drivers_servo_timer
 */
#define SERVO_PARAM_PIN                 GPIO_UNDEF
#endif

#ifndef SERVO_PARAM_MIN_US
/**
 * @brief   Minimum time in µs of a pulse (corresponds to minimum extension)
 *
 * Defaults to `900UL`.
 */
#define SERVO_PARAM_MIN_US              900UL
#endif

#ifndef SERVO_PARAM_MAX_US
/**
 * @brief   Maximum time in µs of a pulse (corresponds to maximum extension)
 *
 * Defaults to `2100UL`.
 */
#define SERVO_PARAM_MAX_US              2100UL
#endif

#ifndef SERVO_PARAMS
/**
 * @brief   Parameters for controlling a servo
 */
#ifdef MODULE_SERVO_PWM
#define SERVO_PARAMS    {\
        .pwm        = SERVO_PARAM_PWM_PARAMS, \
        .min_us     = SERVO_PARAM_MIN_US, \
        .max_us     = SERVO_PARAM_MAX_US, \
        .pwm_chan   = SERVO_PARAM_PWM_CHAN, \
    }
#endif
#ifdef MODULE_SERVO_TIMER
#define SERVO_PARAMS    {\
        .timer      = SERVO_PARAM_TIMER_PARAMS, \
        .servo_pin  = SERVO_PARAM_PIN, \
        .min_us     = SERVO_PARAM_MIN_US, \
        .max_us     = SERVO_PARAM_MAX_US, \
        .timer_chan = SERVO_PARAM_TIMER_CHAN, \
    }
#endif
#endif
/**@}*/

/**
 * @brief   Servo configuration
 */
static const servo_params_t servo_params[] =
{
    SERVO_PARAMS
};

#ifndef SERVO_SAULINFO
/**
 * @brief   Servo SAUL info
 */
#define SERVO_SAULINFO  { .name = "servo" }
#endif

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t servo_saul_info[] =
{
    SERVO_SAULINFO
};

#ifdef __cplusplus
}
#endif

/** @} */
