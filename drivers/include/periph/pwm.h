/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_periph_pwm PWM
 * @ingroup     driver_periph
 * @brief       Low-level PWM peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level PWM peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PWM_H
#define PWM_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

/**
 * @brief Definition of available PWM devices
 *
 * To this point a maximum of four PWM device is available.
 */
typedef enum {
#if PWM_0_EN
    PWM_0,              /*< 1st PWM device */
#endif
#if PWM_1_EN
    PWM_1,              /*< 2nd PWM device */
#endif
#if PWM_2_EN
    PWM_2,              /*< 3rd PWM device */
#endif
#if PWM_3_EN
    PWM_3,              /*< 4th PWM device */
#endif
} pwm_t;

/**
 * @brief Definition of available PWM modes
 */
typedef enum {
    PWM_LEFT,           /*< use left aligned PWM */
    PWM_RIGHT,          /*< use right aligned PWM */
    PWM_CENTER          /*< use center aligned PWM */
} pwm_mode_t;

/**
 * @brief Initialize a PWM device
 *
 * The PWM module is based on virtual PWM devices, which can have one or more channels.
 * The PWM devices can be configured to run with a given frequency and resolution, which
 * are always identical for the complete device, hence for every channel on a device.
 *
 * The desired frequency and resolution may not be possible on a given device when chosen
 * too large. In this case the PWM driver will always keep the resolution and decrease the
 * frequency if needed. To verify the correct settings compare the returned value which
 * is the actually set frequency.
 *
 * @param[in] dev           PWM channel to initialize
 * @param[in] mode          PWM mode, left, right or center aligned
 * @param[in] frequency     the PWM frequency in Hz
 * @param[in] resolution    the PWM resolution
 *
 * @return                  Actual PWM frequency on success
 * @return                  -1 on mode not applicable
 * @return                  -2 on frequency and resolution not applicable
 */
int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution);

/**
 * @brief Set the duty-cycle for a given channel of the given PWM device
 *
 * The duty-cycle is set in relation to the chosen resolution of the given device. If
 * value > resolution, value is set to resolution.
 *
 * @param[in] dev           the PWM device to set
 * @param[in] channel       the channel of the given device to set
 * @param[in] value         the desired duty-cycle to set
 *
 * @return                  0 on success
 * @return                  -1 on invalid channel
 */
int pwm_set(pwm_t dev, int channel, unsigned int value);

/**
 * @brief Start PWM generation on the given device
 *
 * @param[in] dev           device to start
 */
void pwm_start(pwm_t dev);

/**
 * @brief Stop PWM generation on the given device
 *
 * @param[in] dev           device to stop
 */
void pwm_stop(pwm_t dev);

/**
 * @brief Power on the PWM device
 *
 * The PWM deice is powered on. It is dependent on the implementing platform,
 * if the previously set configuration is still available after power on.
 *
 * @param[in] dev           device to power on
 */
void pwm_poweron(pwm_t dev);

/**
 * @brief Power off the given PWM device
 *
 * The given PWM is completely powered off. On most platform this means, that
 * the clock for the PWM device is disabled.
 *
 * @param[in] dev           device to power off
 */
void pwm_poweroff(pwm_t dev);

#endif /* PWM_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* PWM_H */
/** @} */
