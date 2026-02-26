/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_sys_rgb_led RIOT Registry Schema: RGB_LED
 * @ingroup     sys
 * @brief       RIOT Registry RGB_LED Schema representing the basic structure of an RGB LED
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/* RGB-LED */
#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_RGB_LED) || IS_ACTIVE(DOXYGEN)

/**
 * @brief Parameter descriptor for the "red" parameter of the RGB-LED schema.
 */
extern const registry_parameter_t registry_sys_rgb_led_red;

/**
 * @brief Parameter descriptor for the "green" parameter of the RGB-LED schema.
 */
extern const registry_parameter_t registry_sys_rgb_led_green;

/**
 * @brief Parameter descriptor for the "blue" parameter of the RGB-LED schema.
 */
extern const registry_parameter_t registry_sys_rgb_led_blue;

/**
 * @brief Schema descriptor for an RGB-LED.
 */
extern registry_schema_t registry_sys_rgb_led;

/**
 * @brief Schema instance struct of a RGB-LED schema.
 */
typedef struct {
    uint8_t red;       /**< Red color. */
    uint8_t green;     /**< Green color. */
    uint8_t blue;      /**< Blue color. */
} registry_sys_rgb_led_instance_t;

/**
 * @brief Parameter indices for the RGB-LED schema.
 */
typedef const enum {
    REGISTRY_SYS_RGB_LED_RED,   /**< Red color. */
    REGISTRY_SYS_RGB_LED_GREEN, /**< Green color. */
    REGISTRY_SYS_RGB_LED_BLUE,  /**< Blue color. */
} registry_sys_rgb_led_indices_t;

#endif

#ifdef __cplusplus
}
#endif
/** @} */
