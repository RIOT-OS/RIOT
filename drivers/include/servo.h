/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_servo Servo Motor Driver
 * @ingroup     drivers_actuators
 * @brief       High-level driver for servo motors
 *
 * Usage
 * =====
 *
 * Select a flavor of the driver, e.g. `USEMODULE += servo_pwm` for
 * @ref drivers_servo_pwm or `USEMODULE += servo_timer` for
 * @ref drivers_servo_timer to use. Typically, the PWM implementation is the
 * preferred one, but some MCU (e.g. nRF52xxx) cannot configure the PWM
 * peripheral to run anywhere close to the 50 Hz to 100 Hz required.
 *
 * In addition, you many need to extend or adapt @ref servo_params and,
 * depending on the selected implementation, @ref servo_pwm_params or
 * @ref servo_timer_params to match your hardware configuration.
 *
 * The test application in `tests/drivers/servo` can serve as starting point for
 * users.
 *
 * @{
 *
 * @file
 * @brief       High-level driver for easy handling of servo motors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SERVO_H
#define SERVO_H

#include <stddef.h>
#include <stdint.h>

#include "periph/pwm.h"
#include "periph/timer.h"
#include "saul.h"
#include "saul_reg.h"
#include "time_units.h"

#ifndef SERVO_TIMER_MAX_CHAN
/**
 * @brief   In case the `servo_timer` backend is used to driver the servo,
 *          this is the highest channel number usable by the driver
 *
 * @note    To drive *n* servos, *n* + 1 timer channels are required. Hence,
 *          this must be at least 2
 *
 * Trimming this down safes a small bit of RAM: Storage for one pointer is
 * wasted on every servo that could be controlled by a timer but is not
 * actually used.
 */
#define SERVO_TIMER_MAX_CHAN    4
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The SAUL adaption driver for servos
 */
extern const saul_driver_t servo_saul_driver;

/**
 * @brief   PWM configuration parameters for a servos
 *
 * Only used with
 */
typedef struct {
    uint16_t res;           /**< PWM resolution to use */
    uint16_t freq;          /**< PWM frequency to use */
    pwm_t pwm;              /**< PWM dev the servo is connected to */
} servo_pwm_params_t;

/**
 * @brief   Servo device state
 */
typedef struct servo servo_t;

/**
 * @brief   Memory needed for book keeping when using @ref drivers_servo_timer
 */
typedef struct {
    /**
     * @brief   Look up table to get from channel
     *
     * @note    Since timer channel 0 is used to set all servo pins, we use
     *          `chan - 1` as idx rather than `chan` to not waste one entry.
     */
    servo_t *servo_map[SERVO_TIMER_MAX_CHAN];
} servo_timer_ctx_t;

/**
 * @brief   Timer configuration parameters for a servos
 */
typedef struct {
    tim_t timer;            /**< Timer to use */
    uint32_t timer_freq;    /**< Timer frequency to use */
    uint16_t servo_freq;    /**< Servo frequency (typically 50 Hz or 100 Hz) */
    servo_timer_ctx_t *ctx; /**< Per-timer state needed for book keeping */
} servo_timer_params_t;

/**
 * @brief   Configuration parameters for a servo
 */
typedef struct {
#if defined(MODULE_SERVO_PWM) || defined(DOXYGEN)
    /**
     * @brief   Specification of the PWM device the servo is connected to
     *
     * @note    Only available when @ref drivers_servo_pwm is used
     */
    const servo_pwm_params_t *pwm;
#endif
#if defined(MODULE_SERVO_TIMER) || defined(DOXYGEN)
    /**
     * @brief   Specification of the timer to use
     *
     * @note    Only available when @ref drivers_servo_timer is used
     */
    const servo_timer_params_t *timer;
    /**
     * @brief   GPIO pin the servo is connected to
     *
     * @note    Only available when @ref drivers_servo_timer is used
     */
    gpio_t servo_pin;
#endif
    uint16_t min_us;    /**< Duration of high phase (in µs) for min extension */
    uint16_t max_us;    /**< Duration of high phase (in µs) for max extension */
#ifdef MODULE_SERVO_PWM
    /**
     * @brief   PWM channel to use
     *
     * @note    Only available when @ref drivers_servo_pwm is used
     */
    uint8_t pwm_chan;
#endif
#ifdef MODULE_SERVO_TIMER
    /**
     * @brief   Timer channel to use
     *
     * @pre     `(timer_chan > 0) && (timer_chan <= SERVO_TIMER_MAX_CHAN)`
     *
     * @note    Only available when @ref drivers_servo_timer is used
     *
     * The timer channel 0 is used to set the GPIO pin of all servos
     * driver by the timer, the other channels are used to clean the GPIO pin
     * of the corresponding servo according to the current duty cycle.
     */
    uint8_t timer_chan;
#endif
} servo_params_t;

/**
 * @brief   Servo device state
 */
struct servo {
    const servo_params_t *params;   /**< Parameters of this servo */
    /**
     * @brief Minimum PWM duty cycle / timer target matching
     *        @ref servo_params_t::min_us
     *
     * Note that the actual PWM frequency can be significantly different from
     * the requested one, depending on what the hardware can generate using the
     * clock source and clock dividers available.
     */
    uint16_t min;
    /**
     * @brief Maximum PWM duty cycle / timer target matching
     *        @ref servo_params_t::min_us
     *
     * Note that the actual PWM frequency can be significantly different from
     * the requested one, depending on what the hardware can generate using the
     * clock source and clock dividers available.
     */
    uint16_t max;
#ifdef MODULE_SERVO_TIMER
    uint16_t current; /**< Current timer target */
#endif
};

#if defined(MODULE_SERVO_TIMER) || DOXYGEN
/**
 * @brief   Default timer context
 */
extern servo_timer_ctx_t servo_timer_default_ctx;
#endif

/**
 * @brief   Initialize servo
 *
 * @param[out]  dev     Device handle to initialize
 * @param[in]   params  Parameters defining the PWM configuration
 *
 * @retval  0           Success
 * @retval  <0          Failure (as negative errno code to indicate cause)
 */
int servo_init(servo_t *dev, const servo_params_t *params);

/**
 * @brief   Set the servo motor to a specified position
 *
 * The position of the servo is specified in the fraction of maximum extension,
 * with 0 being the lowest extension (e.g. on an 180° servo it would be at -90°)
 * and `UINT8_MAX` being the highest extension (e.g. +90° on that 180° servo).
 *
 * @param[in] dev           the servo to set
 * @param[in] pos           the extension to set
 *
 * Note: 8 bit of resolution may seem low, but is indeed more than high enough
 * for any practical PWM based servo. For higher precision, stepper motors would
 * be required.
 */
void servo_set(servo_t *dev, uint8_t pos);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_H */
/** @} */
