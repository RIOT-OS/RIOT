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
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "registry.h"
#include "registry/namespace/sys.h"

#include "registry/namespace/sys/rgb_led.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_RGB_LED) || IS_ACTIVE(DOXYGEN)

/* Mapping */
static void mapping(const registry_parameter_id_t parameter_id, const registry_instance_t *instance,
                    void **val, size_t *val_len)
{
    registry_sys_rgb_led_instance_t *_instance =
        (registry_sys_rgb_led_instance_t *)instance->data;

    switch (parameter_id) {
    case REGISTRY_SYS_RGB_LED_RED:
        *val = &_instance->red;
        *val_len = sizeof(_instance->red);
        break;

    case REGISTRY_SYS_RGB_LED_GREEN:
        *val = &_instance->green;
        *val_len = sizeof(_instance->green);
        break;

    case REGISTRY_SYS_RGB_LED_BLUE:
        *val = &_instance->blue;
        *val_len = sizeof(_instance->blue);
        break;
    }
}

/* Schema parameters */
const registry_parameter_t registry_sys_rgb_led_red = {
    .id = REGISTRY_SYS_RGB_LED_RED,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "red",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_sys_rgb_led,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

const registry_parameter_t registry_sys_rgb_led_green = {
    .id = REGISTRY_SYS_RGB_LED_GREEN,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "green",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_sys_rgb_led,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

const registry_parameter_t registry_sys_rgb_led_blue = {
    .id = REGISTRY_SYS_RGB_LED_BLUE,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "blue",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_sys_rgb_led,
    .type = REGISTRY_TYPE_UINT8,
    .count = 1,
};

/* Schema */
registry_schema_t registry_sys_rgb_led = {
    .id = REGISTRY_SYS_RGB_LED,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "rgb_led",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .namespace = &registry_sys,
    .mapping = mapping,
    .groups = NULL,
    .groups_len = 0,
    .parameters = (const registry_parameter_t *[]){
        &registry_sys_rgb_led_red,
        &registry_sys_rgb_led_green,
        &registry_sys_rgb_led_blue,
    },
    .parameters_len = 3,
};

#endif
