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
 * This interface enables access to CPU peripherals generating PWM signals. On
 * most platforms, this interface will be implemented based on hardware timers,
 * though some CPUs provide dedicated PWM peripherals.
 *
 * The characteristics of a PWM signal can be defined by three basic parameters,
 * namely the frequency, the duty cycle, and the operational mode. This
 * interface supports basic PWM generation in left-aligned, right-aligned, and
 * center mode. Additionally the interface supports the definition of the used
 * resolution, defining the granularity with which one can specify the duty
 * cycle. This brings more flexibility to the configuration of the frequency,
 * especially on systems with low system clocks.
 *
 * Typically, a single PWM device (e.g. hardware timer) supports PWM signal
 * generation on multiple pins in parallel. While the duty cycle is selectable
 * for each channel individually, the frequency and resolution are shared for
 * all channels.
 *
 * The mapping/configuration of PWM devices (timers) and the used pins has to be
 * done in the board configuration (the board's `periph_conf.h).
 *
 * When using the PWM interface, first thing you have to do is initialize the
 * PWM device with the targeted mode, frequency, and resolution settings. Once
 * the device is initialized, it will start the generation of PWM signals on all
 * configured pins immediately, with an initial duty cycle of `0`. Use the
 * pwm_set() function to change the duty cycle for a given channel. If you
 * want to disable the PWM generation again, simply call pwm_poweroff().
 *
 * ## (Low-)Power implications
 *
 * After initialization, the a PWM peripheral **should** be powered on and
 * active. When manually stopped using the pwm_poweroff() function, the PWM
 * generation **should** be stopped for all channels and the PWM peripheral
 * **should** be fully power off (e.g. through peripheral clock gating). Once
 * being re-enabled by calling the pwm_poweron() function, the PWM peripheral
 * **should** transparently continue its previously configured operation,
 * including the last active duty cycle values.
 *
 * While a PWM device is active, some implementations might need to block
 * certain power modes.
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
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default PWM access macro
 */
#ifndef PWM_DEV
#define PWM_DEV(x)          (x)
#endif

/**
 * @brief  Default PWM undefined value
 */
#ifndef PWM_UNDEF
#define PWM_UNDEF           (UINT_MAX)
#endif

/**
 * @brief  Default number of onboard PWM devices
 */
#ifndef PWM_NUMOF
#define PWM_NUMOF           (0U)
#endif

/**
 * @brief   Default number of PWM extension devices
 */
#ifndef PWM_EXT_NUMOF
#if MODULE_EXTEND_PWM
#define PWM_EXT_NUMOF       (sizeof(pwm_ext_list) / sizeof(pwm_ext_list[0]))
#else
#define PWM_EXT_NUMOF       (0U)
#endif /* MODULE_EXTEND_PWM */
#endif /* PWM_EXT_NUMOF */

/**
 * @brief   Default overall number of PWM devices
 */
#ifndef PWM_DEV_NUMOF
#define PWM_DEV_NUMOF       (PWM_NUMOF + PWM_EXT_NUMOF)
#endif

/**
 * @brief   Default PWM type definition
 */
#ifndef HAVE_PWM_T
typedef unsigned int pwm_t;
#endif

/**
 * @brief   Default PWM mode definition
 */
#ifndef HAVE_PWM_MODE_T
typedef enum {
    PWM_LEFT,           /*< use left aligned PWM */
    PWM_RIGHT,          /*< use right aligned PWM */
    PWM_CENTER          /*< use center aligned PWM */
} pwm_mode_t;
#endif

/**
 * @brief   Low-level versions of the PWM functions
 *
 * These are for cpu pwm.c implementation and should not be called directly.
 * @{
 */
uint32_t pwm_init_cpu(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res);
uint8_t pwm_channels_cpu(pwm_t dev);
void pwm_set_cpu(pwm_t dev, uint8_t channel, uint16_t value);
void pwm_poweron_cpu(pwm_t dev);
void pwm_poweroff_cpu(pwm_t dev);
/** @} */

#if MODULE_EXTEND_PWM || DOXYGEN
/**
 * @brief   Redirecting versions of the PWM functions
 *
 * These are for the extension interface and should not be called directly.
 * @{
 */
uint32_t pwm_init_redir(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res);
uint8_t pwm_channels_redir(pwm_t dev);
void pwm_set_redir(pwm_t dev, uint8_t channel, uint16_t value);
void pwm_poweron_redir(pwm_t dev);
void pwm_poweroff_redir(pwm_t dev);
/** @} */
#endif /* MODULE_EXTEND_PWM || DOXYGEN */

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
static inline uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    (void)dev;
    (void)mode;
    (void)freq;
    (void)res;
#if MODULE_EXTEND_PWM
#if MODULE_PERIPH_PWM
    if (dev >= PWM_NUMOF) {
        return pwm_init_redir(dev, mode, freq, res);
    }
#else
    return pwm_init_redir(dev, mode, freq, res);
#endif /* MODULE_PERIPH_PWM */
#endif /* MODULE_EXTEND_PWM */
#if MODULE_PERIPH_PWM
    return pwm_init_cpu(dev, mode, freq, res);
#else
    return 0;
#endif
}

/**
 * @brief   Get the number of available channels
 *
 * @param[in] dev           PWM device
 *
 * @return                  Number of channels available for the given device
 */
static inline uint8_t pwm_channels(pwm_t dev)
{
    (void)dev;
#if MODULE_EXTEND_PWM
#if MODULE_PERIPH_PWM
    if (dev >= PWM_NUMOF) {
        return pwm_channels_redir(dev);
    }
#else
        return pwm_channels_redir(dev);
#endif /* MODULE_PERIPH_PWM */
#endif /* MODULE_EXTEND_PWM */
#if MODULE_PERIPH_PWM
    return pwm_channels_cpu(dev);
#else
    return 0;
#endif
}

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
static inline void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    (void)dev;
    (void)channel;
    (void)value;
#if MODULE_EXTEND_PWM
#if MODULE_PERIPH_PWM
    if (dev >= PWM_NUMOF) {
        pwm_set_redir(dev, channel, value);
        return;
    }
#else
    pwm_set_redir(dev, channel, value);
    return;
#endif /* MODULE_PERIPH_PWM */
#endif /* MODULE_EXTEND_PWM */
#if MODULE_PERIPH_PWM
    pwm_set_cpu(dev, channel, value);
#endif
}
/**
 * @brief   Resume PWM generation on the given device
 *
 * When this function is called, the given PWM device is powered on and
 * continues its previously configured operation. The duty cycle of each channel
 * will be the value that was last set.
 *
 * This function must not be called before the PWM device was initialized.
 *
 * @param[in] dev           device to start
 */
static inline void pwm_poweron(pwm_t dev)
{
    (void)dev;
#if MODULE_EXTEND_PWM
#if MODULE_PERIPH_PWM
    if (dev >= PWM_NUMOF) {
        pwm_poweron_redir(dev);
        return;
    }
#else
    pwm_poweron_redir(dev);
    return;
#endif /* MODULE_PERIPH_PWM */
#endif /* MODULE_EXTEND_PWM */
#if MODULE_PERIPH_PWM
    pwm_poweron_cpu(dev);
#endif
}

/**
 * @brief   Stop PWM generation on the given device
 *
 * This function stops the PWM generation on all configured channels for the
 * given device and powers down the given PWM peripheral.
 *
 * @param[in] dev           device to stop
 */
static inline void pwm_poweroff(pwm_t dev)
{
    (void)dev;
#if MODULE_EXTEND_PWM
#if MODULE_PERIPH_PWM
    if (dev >= PWM_NUMOF) {
        pwm_poweroff_redir(dev);
        return;
    }
#else
    pwm_poweroff_redir(dev);
    return;
#endif /* MODULE_PERIPH_PWM */
#endif /* MODULE_EXTEND_PWM */
#if MODULE_PERIPH_PWM
    pwm_poweroff_cpu(dev);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_PWM_H */
/** @} */
