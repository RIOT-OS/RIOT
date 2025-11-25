/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       Parameter definitions for mapping peripherals directly to SAUL
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SAUL_PERIPH_H
#define SAUL_PERIPH_H

#if MODULE_SAUL_GPIO || DOXYGEN
#include "periph/gpio.h"
#endif /* MODULE_SAUL_GPIO */

#if MODULE_SAUL_ADC || DOXYGEN
#include "periph/adc.h"
#endif /* MODULE_SAUL_ADC */

#if MODULE_SAUL_PWM || DOXYGEN
#include "periph/pwm.h"
#endif /* MODULE_SAUL_PWM */

#ifdef __cplusplus
extern "C" {
#endif

#if MODULE_SAUL_GPIO || DOXYGEN
/**
 * @brief SAUL GPIO configuration flags
 */
typedef enum {
    SAUL_GPIO_INVERTED   = (1 << 0),  /**< pin is used as inverted */
    SAUL_GPIO_INIT_CLEAR = (1 << 1),  /**< set pin inactive after init */
    SAUL_GPIO_INIT_SET   = (1 << 2),  /**< set pin active after init */
} saul_gpio_flags_t;

/**
 * @brief   Direct mapped GPIO configuration values
 */
typedef struct {
    const char *name;           /**< name of the device connected to this pin */
    gpio_t pin;                 /**< GPIO pin to initialize and expose */
    gpio_mode_t mode;           /**< pin mode to use */
    saul_gpio_flags_t flags;    /**< Configuration flags */
} saul_gpio_params_t;
#endif /* MODULE_SAUL_GPIO */

#if MODULE_SAUL_ADC || DOXYGEN
/**
 * @brief   Direct mapped ADC configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    adc_t line;             /**< ADC line to initialize and expose */
    adc_res_t res;          /**< ADC resolution */
} saul_adc_params_t;
#endif /* MODULE_SAUL_ADC */

#if MODULE_SAUL_PWM || DOXYGEN
/**
 * @brief Resolution of SAUL mapped PWMs
 */
static const uint16_t saul_pwm_resolution = 255;

/**
 * @brief SAUL PWM parameters
 */
typedef enum {
    SAUL_PWM_REGULAR  = (0 << 0),  /**< Physical values are proportional to
                                     average voltage levels (ie. LEDs are in
                                     active-high, anode driven) */
    SAUL_PWM_INVERTED = (1 << 0),  /**< Physical values are inverted from
                                     average voltage levels (ie. LEDs are in
                                     active-low, cathode driven) */
} saul_pwm_flags_t;

/**
 * @brief Single PWM channel exposed via SAUL
 *
 * This does never need to be declared on its own, but is used inisde @ref
 * saul_pwm_dimmer_params_t and @ref saul_pwm_rgb_params_t structs.
 */
typedef struct {
    pwm_t dev;                  /**< PWM device backing this entry */
    uint8_t channel;            /**< Channel on the PWM device */
    saul_pwm_flags_t flags;     /**< Configuration flags */
} saul_pwm_channel_t;

/** @brief Default value for @ref SAUL_PWM_FREQ */
#define SAUL_PWM_FREQ_DEFAULT 1000

/**
 * @brief   Define the PWM frequency for LEDs
 *
 * This frequency is requested from the PWM driver. As the per @ref pwm_init,
 * the actual frequency may be lower, and the SAUL wrapper does not place a
 * limit there.
 *
 * Frequencies of above 200Hz usually give a smooth visual experience. The
 * higher 1kHz is picked as a default as some devices can't go that low with
 * their timer.
 *
 * This is typically set in the board's `pwm_params.h`.
 */
/* This is not applied here as it would later need to be undef'd; actual
 * application of the default happens in auto_init_saul_pwm.c */
#if DOXYGEN
#define SAUL_PWM_FREQ SAUL_PWM_FREQ_DEFAULT
#endif

/**
 * @brief Suppress saul_pwm's dimmer generation
 *
 * This can be defined in `pwm_params.h` if the saul_pwm module is used, but no
 * dimmers (and only RGB LEDs) are in use. Then, no @ref saul_pwm_dimmer_params
 * needs to be set.
 */
#if DOXYGEN
#define SAUL_PWM_NO_DIMMER
#endif

/**
 * @brief   PWM channels mapped to dimmer-style registration entries
 *
 * This is used to define a `static const saul_pwm_dimmer_params_t
 * saul_pwm_dimer_params[]` in a board's `pwm_params.h` for use by the saul_pwm
 * module. If the module is used but only RGB LEDs are present, a @ref
 * SAUL_PWM_NO_DIMMER define can be set instead.
 */
typedef struct {
    const char *name;           /**< Name of the device connected to this
                                  channel */
    saul_pwm_channel_t channel; /**< Full channel description (device, channel)
                                  along with flags that indicate whether high
                                  PWM values are dark or bright*/
} saul_pwm_dimmer_params_t;

/**
 * @brief Suppress saul_pwm's RGB LED generation
 *
 * This can be defined in `pwm_params.h` if the saul_pwm module is used, but no
 * RGB LEDs (and only dimmers) are in use. Then, no @ref saul_pwm_rgb_params_t
 * needs to be set.
 */
#if DOXYGEN
#define SAUL_PWM_NO_RGB
#endif

/**
 * @brief   PWM channels mapped to RGB LED registration entries
 *
 * This is used to define a `static const saul_pwm_rgb_params_t
 * saul_pwm_rgb_params[]` in a board's `pwm_params.h` for use by the saul_pwm
 * module. If the module is used but only dimmers are present, a @ref
 * SAUL_PWM_NO_RGB define can be set instead.
 */
typedef struct {
    const char *name;               /**< Name of the device connected to these
                                     channels */
    saul_pwm_channel_t channels[3]; /**< Full channel description (device, channel)
                                     along with flags that indicate whether high
                                     PWM values are dark or bright*/
} saul_pwm_rgb_params_t;

#endif /* MODULE_SAUL_PWM */

#ifdef __cplusplus
}
#endif

#endif /* SAUL_PERIPH_H */
/** @} */
