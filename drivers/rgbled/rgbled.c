/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_rgbled
 * @{
 *
 * @file
 * @brief       High-level driver for easy handling of RGB-LEDs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "rgbled.h"

/**
 * @name Drive the RGB-LED by default with 500Hz
 */
#define PWM_FREQU       (500)

/**
 * @name Set the default resolution to 8-bit per color (for a 24-bit color space)
 */
#define PWM_RES         (256)

void rgbled_init(rgbled_t *led, pwm_t pwm, int channel_r, int channel_g, int channel_b)
{
    led->device = pwm;
    led->channel_r = channel_r;
    led->channel_g = channel_g;
    led->channel_b = channel_b;
    pwm_init(pwm, PWM_LEFT, PWM_FREQU, PWM_RES);
}

void rgbled_set(const rgbled_t *led, color_rgb_t *color)
{
    pwm_set(led->device, led->channel_r, (unsigned int)color->r);
    pwm_set(led->device, led->channel_g, (unsigned int)color->g);
    pwm_set(led->device, led->channel_b, (unsigned int)color->b);
}
