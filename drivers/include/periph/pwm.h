/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_pwm PWM
 * @ingroup     drivers_periph
 * @brief       Low-level PWM peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level PWM peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_PWM_H
#define PERIPH_PWM_H

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
/* TODO: remove once all platforms are ported to this interface */
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Make sure the number of available PWM devices is defined
 */
#ifndef PWM_NUMOF
#error "PWM_NUMOF undefined for the target platform"
#endif

/**
 * @brief   Default PWM access macro
 * @{
 */
#ifndef PWM_DEV
#define PWM_DEV(x)          (x)
#endif
/** @} */

/**
 * @brief  Default PWM undefined value
 * @{
 */
#ifndef PWM_UNDEF
#define PWM_UNDEF           (-1)
#endif
/** @} */

/**
 * @brief   Default PWM type definition
 * @{
 */
#ifndef HAVE_PWM_T
typedef unsigned int pwm_t;
#endif
/** @} */

/**
 * @brief   Default PWM mode definition
 * @{
 */
#ifndef HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT,           /*< use left aligned PWM */
    PWM_RIGHT,          /*< use right aligned PWM */
    PWM_CENTER          /*< use center aligned PWM */
} pwm_mode_t;
#endif
/** @} */

/**
 * @brief   Initialize a PWM device
 *
 * The PWM module is based on virtual PWM devices, which can have one or more
 * channels. The PWM devices can be configured to run with a given frequency and
 * resolution, which are always identical for the complete device, hence for
 * every channel on a device.
 *
 * The desired frequency and resolution may not be possible on a given device
 * when chosen too large. In this case the PWM driver will always keep the
 * resolution and decrease the frequency if needed. To verify the correct
 * settings compare the returned value which is the actually set frequency.
 *
 * @param[in] dev           PWM device to initialize
 * @param[in] mode          PWM mode, left, right or center aligned
 * @param[in] freq          PWM frequency in Hz
 * @param[in] res           PWM resolution
 *
 * @return                  actual PWM frequency on success
 * @return                  0 on error
 */
uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res);

/**
 * @brief   Get the number of available channels
 *
 * @param[in] dev           PWM device
 *
 * @return                  Number of channels available for the given device
 */
uint8_t pwm_channels(pwm_t dev);

/**
 * @brief   Set the duty-cycle for a given channel of the given PWM device
 *
 * The duty-cycle is set in relation to the chosen resolution of the given
 * device. If value > resolution, value is set to resolution.
 *
 * @param[in] dev           the PWM device to set
 * @param[in] channel       the channel of the given device to set
 * @param[in] value         the desired duty-cycle to set
 */
void pwm_set(pwm_t dev, uint8_t channel, uint16_t value);

/**
 * @brief   Start PWM generation on the given device
 *
 * @param[in] dev           device to start
 */
void pwm_start(pwm_t dev);

/**
 * @brief   Stop PWM generation on the given device
 *
 * @param[in] dev           device to stop
 */
void pwm_stop(pwm_t dev);

/**
 * @brief   Power on the PWM device
 *
 * When the device is powered on the first time, not configuration is set. If
 * the device is powered back on, after having been initialized and powered off
 * before, the PWM device will continue its operation with the previously set
 * configuration. So there is no need in re-initializing then.
 *
 * @param[in] dev           device to power on
 */
void pwm_poweron(pwm_t dev);

/**
 * @brief   Power off the given PWM device
 *
 * This function will power off the given PWM device, which on most platforms
 * means that it will be disabled using clock gating. The implementation must
 * make sure, that any previously configuration will hold when the device is
 * being powered back on.
 *
 * @param[in] dev           device to power off
 */
void pwm_poweroff(pwm_t dev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_PWM_H */
/** @} */
