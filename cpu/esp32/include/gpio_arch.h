/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific GPIO functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_ARCH_H
#define GPIO_ARCH_H

#include "gpio_arch_common.h"
#include "periph/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Current an output pin can drive in active and sleep modes
 */
typedef enum {
    GPIO_DRIVE_5  = 0,    /**<  5 mA */
    GPIO_DRIVE_10 = 1,    /**< 10 mA */
    GPIO_DRIVE_20 = 2,    /**< 20 mA (default) */
    GPIO_DRIVE_30 = 2,    /**< 30 mA */
} gpio_drive_strength_t;

#ifndef DOXYGEN
/**
 * @brief   Table of GPIO to IOMUX register mappings
 */
extern const uint32_t _gpio_to_iomux_reg[];
#define GPIO_PIN_MUX_REG _gpio_to_iomux_reg

/**
 * @brief   Disable the pullup of the pin
 */
void gpio_pullup_dis (gpio_t pin);

/**
 * @brief   Returns the RTCIO pin number or -1 if the pin is not an RTCIO pin
 */
int8_t gpio_is_rtcio (gpio_t pin);

/**
 * @brief   Set the direction of a pin (initializes the pin calling gpio_init)
 * @param   pin     GPIO pin
 * @param   mode    active in sleep mode if true
 * @return  0 on success
 *         -1 on invalid argument
 */
int gpio_set_direction(gpio_t pin, gpio_mode_t mode);

/**
 * @brief   extern declaration of ROM functions to avoid compilation problems
 */
void gpio_matrix_in (uint32_t gpio, uint32_t signal_idx, bool inv);
void gpio_matrix_out(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv);

#endif /* DOXYGEN */

/**
 * @brief   Set the drive-strength of an output-capable pin
 *
 * Sets the drive-strength for an output-capable pin in active and sleep modes.
 * The default drive-strength is GPIO_DRIVE_20 (20 mA).
 *
 * @param   pin     GPIO pin
 * @param   drive   drive-strength
 *                  GPIO_DRIVE_5 for 5 mA
 *                  GPIO_DRIVE_10 for 10 mA
 *                  GPIO_DRIVE_20 for 20 mA (default)
 *                  GPIO_DRIVE_30 for 30 mA
 * @pre     pin is an output-capable pin
 * @post    an assertion blows up if the pin is not output-capable
 * @return  0 on success
 *         -1 on error
 */
int gpio_set_drive_capability(gpio_t pin, gpio_drive_strength_t drive);

/**
 * @brief   Called before the power management enters a light or deep sleep mode
 * @param   mode    sleep mode that is entered
 */
void gpio_pm_sleep_enter(unsigned mode);

/**
 * @brief   Called after the power management left light sleep mode
 * @param   cause   wake-up cause
 */
void gpio_pm_sleep_exit(uint32_t cause);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_ARCH_H */
