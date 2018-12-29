/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_extend_pwm PWM extension
 * @ingroup     drivers_extend
 * @brief       PWM peripheral extension handling
 *
 * The PWM extension interface makes handling of non-CPU PWM devices invisible
 * to code using the PWM periph API (periph/pwm.h). This is accomplished by
 * reserving part of the range of values of pwm_t. When a call to the PWM API
 * uses a line that falls within this range, it is parsed into a device ID that
 * is looked up in the PWM extension list and the call is redirected to the
 * corresponding device.
 *
 * @{
 *
 * @file
 * @brief       PWM extension interface definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef EXTEND_PWM_H
#define EXTEND_PWM_H

#include <limits.h>
#include <stdint.h>

#include "periph_conf.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Convert a PWM device identifier to an extension PWM device identifier
 */
#ifndef PWM_EXT_DEV
#define PWM_EXT_DEV(x)      (x + PWM_NUMOF)
#endif

/**
 * @brief   Callback typedef for pwm_init
 *
 * @see #pwm_init
 */
typedef uint32_t (*pwm_ext_init_t)(void *dev, pwm_mode_t mode, uint32_t freq, uint16_t res);

/**
 * @brief   Callback typedef for pwm_channels
 *
 * @see #pwm_channels
 */
typedef uint8_t (*pwm_ext_channels_t)(void *dev);

/**
 * @brief   Callback typedef for pwm_set
 *
 * @see #pwm_set
 */
typedef void (*pwm_ext_set_t)(void *dev, uint8_t channel, uint16_t value);

/**
 * @brief   Callback typedef for pwm_poweron
 *
 * @see #pwm_poweron
 */
typedef void (*pwm_ext_poweron_t)(void *dev);

/**
 * @brief   Callback typedef for pwm_poweroff_redir
 *
 * @see #pwm_poweroff
 */
typedef void (*pwm_ext_poweroff_t)(void *dev);

/**
 * @brief   Default not supported functions
 * @{
 */
uint32_t pwm_ext_init_notsup(void *dev, pwm_mode_t mode, uint32_t freq, uint16_t res);
uint8_t pwm_ext_channels_notsup(void *dev);
void pwm_ext_set_notsup(void *dev, uint8_t channel, uint16_t value);
void pwm_ext_poweron_notsup(void *dev);
void pwm_ext_poweroff_notsup(void *dev);
/** @} */

/**
 * @brief   PWM extension driver entry
 */
typedef struct pwm_ext_driver {
    pwm_ext_init_t const init;         /**< callback for pwm_ext_init */
    pwm_ext_channels_t const channels; /**< callback for pwm_ext_channels */
    pwm_ext_set_t const set;           /**< callback for pwm_ext_set */
    pwm_ext_poweron_t const poweron;   /**< callback for pwm_ext_poweron */
    pwm_ext_poweroff_t const poweroff; /**< callback for pwm_ext_poweroff */
} pwm_ext_driver_t;

/**
 * @brief   PWM extension list entry
 */
typedef struct pwm_ext {
    pwm_ext_driver_t const *driver;     /**< pointer to driver */
    void *dev;                          /**< pointer to device descriptor */
} pwm_ext_t;

/**
 * @brief   Find an entry in the extension list by PWM dev number
 *
 * @param[in]   dev    PWM device to look up
 * @return      pointer to the list entry
 * @return      NULL if no device is listed
 */
pwm_ext_t *pwm_ext_entry(pwm_t dev);

#ifdef __cplusplus
}
#endif

#endif /* EXTEND_PWM_H */
/** @} */
