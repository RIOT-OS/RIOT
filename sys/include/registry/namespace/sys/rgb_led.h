/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
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

extern const registry_parameter_t registry_sys_rgb_led_red;
extern const registry_parameter_t registry_sys_rgb_led_green;
extern const registry_parameter_t registry_sys_rgb_led_blue;
extern registry_schema_t registry_sys_rgb_led;

typedef struct {
    clist_node_t node;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} registry_sys_rgb_led_instance_t;

typedef const enum {
    REGISTRY_SYS_RGB_LED_RED,
    REGISTRY_SYS_RGB_LED_GREEN,
    REGISTRY_SYS_RGB_LED_BLUE,
} registry_sys_rgb_led_indices_t;

#endif

#ifdef __cplusplus
}
#endif
/** @} */
