/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level PWM periphial driver
 * @{
 *
 * @file        pwm.h
 * @brief       Low-level PWM periphial driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PWM_H
#define __PWM_H

#include "periph_conf.h"


/**
 * @brief Definition of available PWM devices
 */
#ifdef PWM_NUMOF
typedef enum {
#ifdef PWM_0_EN
    PWM_0,              ///< first PWM device
#endif
#ifdef PWM_1_EN
    PWM_1               ///< Second PWM device
#endif
} pwm_t;
#endif

/**
 * @brief Definition of availabe PWM modes
 */
typedef enum {
    PWM_LEFT,           ///< use left aligned PWM
    PWM_RIGHT,          ///< use right aligned PWM
    PWM_CENTER          ///< use center aligned PWM
} pwm_mode_t;


/**
 * @brief Initialize a PWM device
 * 
 * The PWM module is based on virtual PWM devices, which can have one or more channels.
 * The PWM devices can be configured to run with a given frequency and resolution, which
 * are always identical for the complete device, hence for every channel on a device.
 * 
 * The desired frequency and resolution may not be possible on a given device when chosen
 * to large. In this case the PWM driver will always keep the resolution and decrease the
 * frequency if needed. To veryfy the correct settings compare the returned value which
 * is the actually set frequency.
 * 
 * @param dev           PWM channel to initialize
 * @param mode          PWM mode, left, right or center aligned
 * @param frequency     the PWM frequency in Hz
 * @param resolution    the PWM resolution
 * @return              the acutally set frequency, 
 *                      0 if frequency and resolution settings are not applicable,
 *                      -1 on error
 */
int pwm_init(pwm_t dev, pwm_mode_t mode, unsigned int frequency, unsigned int resolution);

/**
 * @brief Set the duty-cycle for a given channel of the given PWM device
 * 
 * The duty-cycle is set in relation to the chosen resolution of the given device. If
 * value > resolution, value is set to resolution.
 * 
 * @param dev           the PWM device to set
 * @param channel       the channel of the given device to set
 * @param value         the desired duty-cycle to set
 * @return              the acutally set duty-cycle, -1 on invalic device or channel
 */
int pwm_set(pwm_t dev, int channel, unsigned int value);

/**
 * @brief Start PWM generation on the given device
 * 
 * @param dev           device to start
 */
void pwm_start(pwm_t dev);

/**
 * @brief Stop PWM generation on the given deivce
 * 
 * @param dev           device to stop
 */
void pwm_stop(pwm_t dev);


#endif /* __PWM_H */
/** @} */
