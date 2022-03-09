/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_rgbled RGB-LED driver
 * @ingroup     drivers_actuators
 * @brief       High-level driver for RGB-LEDs
 * @{
 *
 * @file
 * @brief       High-level driver for easy handling of RGB-LEDs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef RGBLED_H
#define RGBLED_H

#include "color.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Descriptor struct for rgbleds
 */
typedef struct {
    pwm_t device;       /**< PWM device to use to control rgbleds */
    int channel_r;      /**< set the R value in RGB */
    int channel_g;      /**< set the G value in RGB */
    int channel_b;      /**< set the B value in RGB */
} rgbled_t;

/**
 * @brief   Initialize the RGB LED by assigning PWM channels to colors
 *
 * @param[in] led           Struct identifying the LED
 * @param[in] pwm           PWM device to drive the LED
 * @param[in] channel_r     PWM channel connected to red
 * @param[in] channel_g     PWM channel connected to green
 * @param[in] channel_b     PWM channel connected to blue
 */
void rgbled_init(rgbled_t *led, pwm_t pwm, int channel_r, int channel_g, int channel_b);

/**
 * @brief   Set the RGB-LED to the given color value
 *
 * @param[in] led           Struct identifying the LED to set
 * @param[in] color         Color to set the led to
 */
void rgbled_set(const rgbled_t *led, color_rgb_t *color);

#ifdef __cplusplus
}
#endif

#endif /* RGBLED_H */
/** @} */
